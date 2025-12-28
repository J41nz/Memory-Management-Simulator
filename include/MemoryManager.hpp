#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include "MemoryBlock.hpp"
#include <vector>
#include <string>

class MemoryManager{
    private:
        size_t total_size; //total physical memory simulated
        MemoryBlock* head; //start of memory list
        std::string current_strategy; // first fit, best fit or worst fit

    public:
        MemoryManager(size_t size); //declaration of constructor. Initialize memory
        ~MemoryManager(); //Cleanup memory to prevent memory leaks!
        
        void set_strategy(std::string strategy);
        void display_stats();
        //Core functions to be implemented later
        void* allocate(size_t size, int process_id);
        void deallocate(int process_id);

        //Visualization
        void dump_memory();
};

#endif