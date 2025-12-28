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

void MemoryManager::set_strategy(std::string strategy){
    current_strategy = strategy;
}

//the allocation logic
void* MemoryManager::allocate(size_t request_size, int process_id){
    MemoryBlock* selected_block = nullptr;
    
    if(current_strategy == "First Fit"){
        MemoryBlock* current = head;
        while(current!=nullptr){
            if(current->is_free && current->size>=request_size){
                selected_block = current;
                break;
            }
            current = current->next;
        }
    }
    else if(current_strategy == "Best Fit"){
        MemoryBlock* current = head;
        size_t smallest_so_far = 9999999;
        while(current!=nullptr){
            if(current->is_free && current->size>=request_size){
                if(current->size < smallest_so_far){
                    smallest_so_far = current -> size;
                    selected_block = current;
                }
            }
            current = current->next;
        }
    }
    else if(current_strategy == "Worst Fit"){
        MemoryBlock* current = head;
        size_t largest_so_far = 0;
        while(current!=nullptr){
            if(current->is_free && current->size>=request_size){
                if(current->size>largest_so_far){
                    largest_so_far = current->size;
                    selected_block = current;
                }
            }
            current = current->next;
        }
    }

    //Now after we find a block, using the split logic to split the memory to use only the required amount of memory
    if(selected_block){
        if(selected_block->size > request_size){
            MemoryBlock* new_free_block = new MemoryBlock(
                selected_block->start_address+request_size,
                selected_block->size-request_size, 
                true, -1
            );
            new_free_block->next = selected_block->next;
            new_free_block->prev = selected_block;
            if(selected_block->next) selected_block->next->prev = new_free_block;
            selected_block->next = new_free_block;
            selected_block->size = request_size;
        }
        selected_block->is_free = false;
        selected_block->process_id = process_id;
        return (void*)selected_block->start_address;
    }

    return nullptr;
}

void MemoryManager::deallocate(int process_id){
    MemoryBlock* current = head;
    bool found = false;

    while(current!=nullptr){
        //find the blockassigned to this process 
        if(!current->is_free && current->process_id==process_id){
            current->is_free=true;
            current->process_id = -1;
            found = true;

            //coalescing logic
            //1. Checks if the next block is also free
            if(current->next!=nullptr && current->next->is_free){
                MemoryBlock* temp = current->next;
                current->size+=temp->size;
                current->next = temp->next;
                if(temp->next!=nullptr){
                    temp->next->prev = current;
                }
                delete temp;
            }

            //2. checks if previous block is also free
            if(current->prev!=nullptr && current->prev->is_free){
                MemoryBlock* prev_block = current->prev;
                prev_block->size+=current->size; 
                prev_block->next = current->next;
                if(current->next != nullptr){
                    current->next->prev = prev_block;
                }
                delete current;                
            }

            std::cout << "Process " << process_id << " deallocated and memory coalesced.\n";
            return; 
        }
        current = current->next;
    }

    if(!found){
        std::cout << "Error: Process ID " << process_id << " not found.\n";
    }
}

void MemoryManager::display_stats(){
    size_t used_memory = 0;
    size_t free_memory = 0;
    size_t largest_free_block = 0;
    int free_block_count = 0;

    MemoryBlock* current = head;
    while(current!=nullptr){
        if(current->is_free){
            free_memory += current->size;
            if(current->size > largest_free_block){
                largest_free_block = current->size;
            }
            free_block_count++;
        } else {
            used_memory += current->size;
        }
        current = current->next;
    }

    double utilization = (static_cast<double>(used_memory)/total_size) * 100.0;

    //External fragmentation calculation
    double fragmentation = 0;
    if(free_memory>0){
        fragmentation = (1.0 - (static_cast<double>(largest_free_block)/free_memory)) * 100.0;
    }

    std::cout << "\n========== MEMORY STATISTICS ==========\n";
    std::cout << "Total Memory:    " << total_size << " bytes\n";
    std::cout << "Used Memory:     " << used_memory << " bytes\n";
    std::cout << "Free Memory:     " << free_memory << " bytes\n";
    std::cout << "Utilization:     " << std::fixed << std::setprecision(2) << utilization << "%\n";
    std::cout << "External Frag:   " << fragmentation << "%\n";
    std::cout << "Free Block Count: " << free_block_count << "\n";
    std::cout << "=======================================\n";
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