#ifndef COMMON_HPP
#define COMMON_HPP

namespace ustcadb {

#define FRAMESIZE 4096
#define DEFBUFSIZE 1024

/**
 * @brief buffer frame
 */
struct bFrame {
    char field[FRAMESIZE];
};

/**
 * @brief buffer control block
 */
struct BCB {
    BCB() = default;
    int page_id;
    int frame_id;
    int count;
    int dirty;
    // Hash Overflow Chain
    BCB *next;
    // LRU List
    BCB *lru_prev;  // the prior node of the LRU List
    BCB *lru_next;  // the next node of the LRU List
};

extern bFrame buf[DEFBUFSIZE];

}  // namespace ustcadb

#endif  // COMMON_HPP