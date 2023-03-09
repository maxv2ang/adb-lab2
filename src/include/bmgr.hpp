#ifndef BMGR_HPP
#define BMGR_HPP

#include <string>

#include "common.hpp"
#include "dsmgr.hpp"

namespace ustcadb {

/**
 * @brief Buffer Manager
 */
class BMgr {
   public:
    BMgr(std::string filename);
    ~BMgr();
    // Interface functions
    /**
     * @brief Look to see if the page is in the buffer already and return the
     * corresponding frame_id if it is. If the page is not resident in the buffer yet,
     * it selects a victim page, if needed, and loads in the requested page.
     * @param page_id id of the page
     * @param type true if it is a write operation, or else it is a read operation
     * @return id of the frame
     */
    int FixPage(int page_id, int type);
    /**
     * @brief Find an empty page that the Disk and Storage Manager can use to
     * store some data.
     * @param page_id id of the page
     * @return id of the frame
     */
    int FixNewPage(int page_id);
    /**
     * @brief decreace the fix count on the frame
     * @param page_id id of the page
     * @return id of the frame
     */
    int UnfixPage(int page_id);
    /**
     * @brief Look at the buffer and figure out the number of buffer pages
     * that are free and able to be used.
     * @return num of the free frames that can be used
     */
    int NumFreeFrames();
    /**
     * @brief Get the num of pages for test use.
     * @return num of pages
     */
    int GetNumPages();
    /**
     * @brief Print infomation about hit rate and I/Os.
     */
    void PrintInfo();

   private:
    // Internal Functions
    /**
     * @brief select a frame to replace.
     * @return a pointer to a buffer control block of the selected frame
     */
    BCB *SelectVictim();
    /**
     * @brief Use the simple Static Hashing technique.
     * H(k) = (page_id)%buffer_size
     * @param page_id id of the page
     * @return the value after hashing of page_id
     */
    int Hash(int page_id);
    /**
     * @brief Remove the buffer control block. If the dirty field is set,
     * the page needs to be written on to the disk.
     * @param ptr a pointer to the buffer control block
     */
    void RemoveBCB(BCB *ptr);
    /**
     * @brief set the frame_id rank value of the ftop array to -1.
     * @param frame_id id of the buffer frame
     */
    void RemoveBFrame(int frame_id);
    /**
     * @brief Set the dirty field of the buffer control block to true.
     * @param ptr a pointer to the buffer control block
     */
    void SetDirty(BCB *ptr);
    /**
     * @brief Set the dirty field of the buffer control block to false.
     * @param ptr a pointer to the buffer control block
     */
    void UnsetDirty(BCB *ptr);
    /**
     * @brief The purpose of the function is to write out any pages that are still
     * in the buffer that may need to be written.
     */
    void WriteDirtys();
    // void PrintFrame(int frame_id);

   private:
    DSMgr *dsmgr;  // an object of Disk and Storage Manager
    // Hash Table
    int ftop[DEFBUFSIZE];   // frame_id to page_id
    BCB *ptof[DEFBUFSIZE];  // page_id to frame_id
    // LRU_List
    BCB *newest;  // the newest frame that used
    BCB *oldest;  // the least unused frame
    // Buffer Manager
    int num_frames;    // the num of frames that used
    int num_requests;  // the num of requests
    int num_hits;      // the num of hits
    int num_read;      // the num of read from disk
    int num_write;     // the num of write to disk
};

}  // namespace ustcadb

#endif  // BMGR_HPP