#include "dsmgr.hpp"

#include <cstdio>
#include <string>

namespace ustcadb {

DSMgr::DSMgr(std::string filename) { OpenFile(filename); }

DSMgr::~DSMgr() { CloseFile(); }

int DSMgr::OpenFile(std::string filename) {
    currFile = fopen(filename.c_str(), "r+");
    if (currFile == NULL) {
        currFile = fopen(filename.c_str(), "w+");
    }
    fseek(currFile, 0L, SEEK_END);
    long size = ftell(currFile);
    numPages = size / FRAMESIZE;

    return currFile != nullptr;
}

int DSMgr::CloseFile() { return fclose(currFile); }

void DSMgr::ReadPage(int page_id, bFrame &frm) {
    if (page_id > numPages) {
        fprintf(stderr, "Error: cannot find page: %d\n", page_id);
        exit(1);
    }
    fseek(currFile, page_id * FRAMESIZE, SEEK_SET);
    fread(frm.field, 1, FRAMESIZE, currFile);
}

void DSMgr::WritePage(int page_id, bFrame &frm) {
    if (page_id > numPages) {
        fprintf(stderr, "Error: cannot find page: %d\n", page_id);
        exit(1);
    }
    fseek(currFile, page_id * FRAMESIZE, SEEK_SET);
    fwrite(frm.field, 1, FRAMESIZE, currFile);
}

void DSMgr::IncNumPages() { ++numPages; }

int DSMgr::GetNumPages() { return numPages; }

}  // namespace ustcadb