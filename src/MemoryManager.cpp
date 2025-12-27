#include "./../include/MemoryManager.hpp"
#include <iomanip>

//Constructor: Initializes the simulation with one giant FREE block
MemoryManager::MemoryManager(size_t size) : total_size(size), current_strategy("First Fit") {
    //create the initial block representing all of memory
    head = new MemoryBlock(0, size, true, -1);
}

//Destructor: Clean up the linked list to avoid memory leaks
MemoryManager::~MemoryManager(){
    MemoryBlock* current = head;
    while(current!=nullptr){
        MemoryBlock* next = current->next;
        delete current;
        current = next;
    }
}

//the allocation logic
void* MemoryManager::allocate(size_t request_size, int process_id){
    MemoryBlock* current = head;

    //First fit - loop through the list until we find a block that fits
    while(current!=nullptr){
        if(current->is_free && current->size >= request_size){
            //if the block is significantly larger than requested, split it.
            if(current->size > request_size){
                //Create a new "remainder" block
                MemoryBlock* new_free_block = new MemoryBlock(
                    current->start_address+request_size, //Starts right after the allocated part
                    current->size - request_size, //remaining size
                    true, //it is free
                    -1  //no process owns it
                );

                //Insert the new block into our doubly linked list
                new_free_block->next = current->next;
                new_free_block->prev = current;
                if(current->next!=nullptr){
                    current->next->prev = new_free_block;
                }
                current->next = new_free_block;
                //update the current block to reflect its new size
                current->size = request_size;
            }

            //Mark the current block as used
            current->is_free = false;
            current->process_id = process_id;

            //Return the simulated "address" (cast to void* to mimic malloc)
            return (void*)current->start_address;
        }
        current = current->next;
    }

    //If we reach here, no block was found (Out of memory)
    return nullptr;
}

//Simple Visualization for debugging
void MemoryManager::dump_memory(){
    MemoryBlock* current = head;
    std::cout << "\n----Memory Dump----\n";
    while(current!=nullptr){
        std::cout << "[Addr: " << std::setw(4) << current->start_address
                  << " | Size: " << std::setw(4) << current->size
                  << " | " << (current->is_free ? "FREE" : "USED (PID:" + std::to_string(current->process_id) + ")")
                  << "]\n";
        current = current->next;
    }
    std::cout << "--------------------\n";
}