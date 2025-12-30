#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include "MemoryBlock.hpp"
#include "Cache.hpp"
#include "PageTable.hpp"
#include <unordered_map>
#include <vector>
#include <list>
#include <string>

class MemoryManager{
    private:
        size_t total_size; //total physical memory simulated
        MemoryBlock* head; //start of memory list
        std::string current_strategy; // first fit, best fit or worst fit

        //Cache hierarchy
        Cache* l1_cache;
        Cache* l2_cache;

        //Virtual Memory Constants
        size_t page_size = 256;
        size_t physical_memory_size; // The RAM size
        size_t total_frames;

        // Virtual memory structures

        //1. The collection of Page Tables (One per Process ID)
        std::unordered_map<int, PageTable*> process_page_tables;

        //Global frame table (to track which process owns which frame)
        //-1 means free. >=0 means PID
        std::vector<int> frame_table;
        std::list<int> active_frames_fifo; // Tracks allocated frames for FIFO eviction

        //Metrics
        size_t internal_fragmentation = 0;
        size_t total_allocs = 0;
        size_t successful_allocs = 0;
        size_t failed_allocs = 0;

    public:
        MemoryManager(size_t size); //declaration of constructor. Initialize memory
        ~MemoryManager(); //Cleanup memory to prevent memory leaks!
        
        //Core functions
        long long allocate(size_t size, int process_id);
        void deallocate(int process_id);
        void access_memory(size_t virtual_addr, int pid);

        //Helpers   
        int get_free_frame_or_evict(int pid);
        size_t virtual_to_physical(int pid, size_t virtual_addr);

        // ... stats, strategy setter, etc ... 
        void set_strategy(std::string strategy);
        void display_stats();
        size_t next_power_of_two(size_t n);
        void dump_memory();
};

#endif