#ifndef DSMGR_HPP
#define DSMGR_HPP

#include <cstdio>
#include <string>

#include "common.hpp"

namespace ustcadb {

/**
 * @brief Disk and Storage Manager
 */
class DSMgr {
   public:
    DSMgr(std::string filename);
    ~DSMgr();
    /**
     * @brief open the database file.
     * @param filename name of the database file
     * @return 1 if the file is successfully opened
     */
    int OpenFile(std::string filename);
    /**
     * @brief close the database file.
     * @return 1 if the file is successfully closed
     */
    int CloseFile();
    /**
     * @brief read the page from disk.
     * @param page_id id of the page
     * @param frm frame data
     */
    void ReadPage(int page_id, bFrame &frm);
    /**
     * @brief write the page to disk.
     * @param page_id id of the page
     * @param frm frame data
     */
    void WritePage(int page_id, bFrame &frm);
    /**
     * @brief increase numPages by 1.
     */
    void IncNumPages();
    /**
     * @brief Get the num of pages.
     * @return num of pages
     */
    int GetNumPages();

   private:
    FILE *currFile;
    int numPages;
};

}  // namespace ustcadb

#endif  // DSMGR_HPP