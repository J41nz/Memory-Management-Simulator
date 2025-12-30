#include "../include/PageTable.hpp"

PageTable::PageTable(size_t pg_size) : page_size(pg_size){}

int PageTable::translate(int page_num){
    if(entries.find(page_num)!=entries.end() && entries[page_num].valid){
        return entries[page_num].frame_number;
    }
    return -1; //Indicates a page fault
}

void PageTable::map(int page_num, int frame_num){
    entries[page_num] = {frame_num, true};
}

void PageTable::invalidate_frame(int frame_num){
    for(auto& pair : entries){
        //Find the page that points to the victim frame
        if(pair.second.frame_number == frame_num && pair.second.valid){
            pair.second.valid = false; // Mark as on DISK
            std::cout << "[PAGE TABLE] Page " << pair.first << " is now INVALID (evicted from frame " << frame_num << ")\n";
            return;
        }
    }
}