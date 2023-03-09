#include "bmgr.hpp"

#include <cstdio>
#include <string>

namespace ustcadb {

bFrame buf[DEFBUFSIZE];

BMgr::BMgr(std::string filename) {
    dsmgr = new DSMgr(filename);

    memset(ftop, -1, sizeof(ftop));
    memset(ptof, 0, sizeof(ptof));

    newest = nullptr;
    oldest = nullptr;

    num_frames = 0;
    num_requests = 0;
    num_hits = 0;
    num_read = 0;
    num_write = 0;
}

BMgr::~BMgr() {
    WriteDirtys();
    delete dsmgr;
}

int BMgr::FixPage(int page_id, int type) {
    BCB *ptr;
    ++num_requests;

    // find the bcb according to the page_id
    int hindex = Hash(page_id);
    BCB *hptr = ptof[hindex];
    while (hptr != nullptr && hptr->page_id != page_id) {
        hptr = hptr->next;
    }
    // if the statement is true, this page is not in the buffer yet.
    if (hptr == nullptr) {
        ++num_read;
        // check if the buffer is full
        if (num_frames == DEFBUFSIZE) {
            RemoveBCB(SelectVictim());
            --num_frames;
        }
        ++num_frames;

        // insert bcb to the hashing table
        ptr = new BCB();
        ptr->next = ptof[hindex];
        ptof[hindex] = ptr;

        // insert bcb to the lru list
        if (oldest == nullptr) {
            oldest = ptr;
            oldest->lru_next = nullptr;
        } else {
            newest->lru_prev = ptr;
            ptr->lru_next = newest;
        }
        newest = ptr;
        newest->lru_prev = nullptr;

        // set the ftop array
        while (ftop[hindex] != -1) {
            hindex = (hindex + 1) % DEFBUFSIZE;
        }
        ftop[hindex] = page_id;

        ptr->frame_id = hindex;
        ptr->page_id = page_id;
        ptr->count = 0;

        // read page from disk
        dsmgr->ReadPage(ptr->page_id, buf[ptr->frame_id]);
    } else {
        ++num_hits;
        ptr = hptr;

        // if ptr is not newest, put it there.
        if (newest != ptr) {
            if (ptr->lru_prev != nullptr) {
                ptr->lru_prev->lru_next = ptr->lru_next;
            }
            if (ptr->lru_next != nullptr) {
                ptr->lru_next->lru_prev = ptr->lru_prev;
            }
            if (oldest == ptr) {
                oldest = ptr->lru_prev;
            }
            newest->lru_prev = ptr;
            ptr->lru_next = newest;
            newest = ptr;
            newest->lru_prev = nullptr;
        }
    }

    ptr->dirty = type;
    ++ptr->count;

    return ptr->frame_id;
}

int BMgr::FixNewPage(int page_id) {
    BCB *ptr = new BCB();

    // insert bcb to the hasing table
    int hindex = Hash(page_id);
    ptr->next = ptof[hindex];
    ptof[hindex] = ptr;

    // check if the buffer is full
    if (num_frames == DEFBUFSIZE) {
        RemoveBCB(SelectVictim());
        --num_frames;
    }
    ++num_frames;

    // set the ftop array
    while (ftop[hindex] != -1) {
        hindex = (hindex + 1) % DEFBUFSIZE;
    }
    ftop[hindex] = page_id;

    ptr->frame_id = hindex;
    ptr->page_id = page_id;
    ptr->count = 0;
    ptr->dirty = true;

    // update lru list
    if (oldest == nullptr) {
        oldest = ptr;
    } else {
        newest->lru_prev = ptr;
    }
    ptr->lru_next = newest;
    newest = ptr;
    ptr->lru_prev = nullptr;

    // put some data to the buffer frame
    memset(buf[ptr->frame_id].field, 0, sizeof(bFrame));

    dsmgr->IncNumPages();

    return ptr->frame_id;
}

int BMgr::UnfixPage(int page_id) {
    // find the bcb according to the page_id
    int hindex = Hash(page_id);
    BCB *hptr = ptof[hindex];
    while (hptr != nullptr && hptr->page_id != page_id) {
        hptr = hptr->next;
    }
    if (hptr == nullptr) {
        fprintf(stderr, "Error: buffer control block not found (UnfixPage)\n");
        exit(1);
    }

    --hptr->count;
    return hptr->frame_id;
}

int BMgr::NumFreeFrames() { return DEFBUFSIZE - num_frames; }

int BMgr::GetNumPages() { return dsmgr->GetNumPages(); }

void BMgr::PrintInfo() {
    printf("total requests: %d\n", num_requests);
    printf("num of hits: %d\n", num_hits);
    printf("num of read from disk: %d\n", num_read);
    printf("num of write to disk: %d\n", num_write);
    printf("total I/Os: %d\n", num_read + num_write);
    printf("hit rate: %lf\n", (double)num_hits / (double)num_requests);
}

BCB *BMgr::SelectVictim() { return oldest; }

int BMgr::Hash(int page_id) { return page_id % DEFBUFSIZE; }

void BMgr::RemoveBCB(BCB *ptr) {
    // find the bcb in the hashing table
    int hindex = Hash(ptr->page_id);
    BCB *hptr = ptof[hindex];
    if (hptr == ptr) {
        ptof[hindex] = ptr->next;
    } else {
        while (hptr->next != nullptr && hptr->next != ptr) {
            hptr = hptr->next;
        }
        if (hptr->next == nullptr) {
            fprintf(stderr, "Error: buffer control block not found (RemoveBCB)\n");
            exit(1);
        }
        hptr->next = ptr->next;
    }

    // update lru list
    if (ptr == oldest) {
        oldest = ptr->lru_prev;
        if (oldest != nullptr) {
            oldest->lru_next = nullptr;
        }
    } else if (ptr == newest) {
        newest = ptr->lru_next;
        if (newest != nullptr) {
            newest->lru_prev = nullptr;
        }
    }
    if (ptr->lru_prev != nullptr) {
        ptr->lru_prev->lru_next = ptr->lru_next;
    }
    if (ptr->lru_next != nullptr) {
        ptr->lru_next->lru_prev = ptr->lru_prev;
    }

    if (ptr->dirty == 1) {
        dsmgr->WritePage(ptr->page_id, buf[ptr->frame_id]);
        ++num_write;
    }

    RemoveBFrame(ptr->frame_id);

    delete ptr;
}

void BMgr::RemoveBFrame(int frame_id) { ftop[frame_id] = -1; }

void BMgr::SetDirty(BCB *ptr) { ptr->dirty = true; }

void BMgr::UnsetDirty(BCB *ptr) { ptr->dirty = false; }

void BMgr::WriteDirtys() {
    while (oldest != nullptr) {
        RemoveBCB(SelectVictim());
    }
}

}  // namespace ustcadb