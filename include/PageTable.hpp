#ifndef PAGE_TABLE_HPP
#define PAGE_TABLE_HPP

#include <unordered_map>
#include <vector>

struct PageTableEntry{
    int frame_number;
    bool valid; // true if the page is in physical memory
};

class PageTable{
private:
    //Maps Page Number -> PageTableEntry
    std::unordered_map<int, PageTableEntry> table;
    size_t page_size;

public:
    PageTable(size_t pg_size);

    //Maps a virtual page to a physical frame
    void map(int page_num, int frame_num);

    //Return frame number, or -1 if Page Fault
    int translate(int page_num);

    size_t get_page_size(){ return page_size; }
};

#endif