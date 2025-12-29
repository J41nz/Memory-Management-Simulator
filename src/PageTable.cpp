#include "../include/PageTable.hpp"

PageTable::PageTable(size_t pg_size) : page_size(pg_size){}

void PageTable::map(int page_num, int frame_num){
    table[page_num] = {frame_num, true};
}

int PageTable::translate(int page_num){
    if(table.find(page_num)!=table.end() && table[page_num].valid){
        return table[page_num].frame_number;
    }
    return -1; //Indicates a page fault
}

