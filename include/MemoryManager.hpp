#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include "MemoryBlock.hpp"
#include "Cache.hpp"
#include "PageTable.hpp"
#include <vector>
#include <string>

class MemoryManager{
    private:
        size_t total_size; //total physical memory simulated
        MemoryBlock* head; //start of memory list
        std::string current_strategy; // first fit, best fit or worst fit
        Cache* l1_cache;
        Cache* l2_cache;
        size_t page_size = 256;
        std::unordered_map<int, PageTable*> process_page_tables;

    public:
        MemoryManager(size_t size); //declaration of constructor. Initialize memory
        ~MemoryManager(); //Cleanup memory to prevent memory leaks!
        
        size_t virtual_to_physical(int pid, size_t virtual_addr);
        void access_memory(size_t address);
        void set_strategy(std::string strategy);
        void display_stats();
        //Core functions to be implemented later
        long long allocate(size_t size, int process_id);
        void deallocate(int process_id);

        //Visualization
        void dump_memory();
};

#endif