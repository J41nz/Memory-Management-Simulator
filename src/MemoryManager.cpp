#include "./../include/MemoryManager.hpp"
#include <iomanip>
#include <limits>
#include <thread>
#include <chrono>

//Constructor: Initializes the simulation with one giant FREE block
MemoryManager::MemoryManager(size_t size) : total_size(size), current_strategy("First Fit") {
    // Physical Memory Setup
    physical_memory_size = size;
    total_frames = physical_memory_size / page_size;
    frame_table.assign(total_frames, -1); // All frames free (-1)

    // Virtual Memory Manager: The 'head' list manages VIRTUAL space.
    head = new MemoryBlock(0, 65536, true, -1);

    //Cache: 128B Size, 16B block, 2-way set associative
    l1_cache = new Cache(128, 16, 2);
    l2_cache = new Cache(512, 16, 4);
}

//Destructor: Clean up the linked list to avoid memory leaks
MemoryManager::~MemoryManager(){
    MemoryBlock* current = head;
    while(current!=nullptr){
        MemoryBlock* next = current->next;
        delete current;
        current = next;
    }
    delete l1_cache;
    delete l2_cache;
    for(auto& pair : process_page_tables){
        delete pair.second;
    }
    process_page_tables.clear();
}

int MemoryManager::get_free_frame_or_evict(int pid){
    //1. Check for free frames
    for(int i=0; i<(int)total_frames; i++){
        if(frame_table[i] == -1){
            frame_table[i] = pid;
            active_frames_fifo.push_back(i);
            return i;
        }
    }

    //2. If NO free frame -> Eviction (FIFO)
    std::cout << "[DISK I/O] Physical Memory Full. Evicting victim page...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); //Simulate disk latency

    //A. Pick victim framefrom the front of FIFO queue
    int victim_frame = active_frames_fifo.front();
    active_frames_fifo.pop_front();

    //B. Find out who owned this frame
    int victim_pid = frame_table[victim_frame];

    //C. Update victim's page table
    //This is basically telling the old process that its address "X" is no longer in RAM
    if(victim_pid!=-1 && process_page_tables.find(victim_pid)!=process_page_tables.end()){
        process_page_tables[victim_pid]->invalidate_frame(victim_frame);
        std::cout << " -> Evicted PID " << victim_pid << " from Frame " << victim_frame << "\n";
    }

    //D. Assign frame to new process 
    frame_table[victim_frame] = pid;
    active_frames_fifo.push_back(victim_frame);

    return victim_frame;
}

size_t MemoryManager::virtual_to_physical(int pid, size_t virtual_addr){
    // 1. If process doesn't have a page table, create one
    if(process_page_tables.find(pid) == process_page_tables.end()){
        process_page_tables[pid] = new PageTable(page_size);
    }

    size_t page_num = virtual_addr / page_size;
    size_t offset = virtual_addr % page_size;

    int frame_num = process_page_tables[pid]->translate(page_num);

    if(frame_num==-1){
        // PAGE FAULT HANDLING
        std::cout << "[PAGE FAULT] Process " << pid << " accessed Page " << page_num << "\n";

        frame_num = get_free_frame_or_evict(pid);

        //Update Page Table
        process_page_tables[pid]->map(page_num, frame_num);
        std::cout << "[PAGE FAULT HANDLED] Mapped V-Page " << page_num << " -> P-Frame " << frame_num << "\n";
    }
     return (frame_num * page_size) + offset;
}

void MemoryManager::access_memory(size_t virtual_addr, int pid){
    std::cout << "\n[CPU Request] Process " << pid << " requesting Virtual Address 0x"
              << std::hex << virtual_addr << std::dec << "...\n";

    //Step 1: Translate Virtual ->physical (handles page faults)
    size_t physical_addr = virtual_to_physical(pid, virtual_addr);

    std::cout << " -> Physical Address: 0x" << std::hex << physical_addr << std::dec << "\n";

    //Step 2: Access caches using physical address 
    if(l1_cache->access(physical_addr)){
        std::cout << " -> L1 Cache Hit!\n";
    }
    else if(l2_cache->access(physical_addr)){
        std::cout << " -> L1 miss, L2 Cache Hit!\n";
    }
    else{
        std::cout << " -> L1 miss, L2 miss. Fetching from Main Memory...\n";
    }
}

void MemoryManager::set_strategy(std::string strategy){
    current_strategy = strategy;
}

size_t MemoryManager::next_power_of_two(size_t n){
    size_t val = 1;
    while(val<n) val<<=1;
    return val;
}

//the allocation logic
long long MemoryManager::allocate(size_t request_size, int process_id){
    total_allocs++;
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
        size_t smallest_so_far = std::numeric_limits<size_t>::max();
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
    else if(current_strategy == "Buddy"){
        size_t target_size = next_power_of_two(request_size);
        MemoryBlock* current = head;
        MemoryBlock* best_buddy = nullptr;

        // 1. Finds the smallest block that is >= target_size
        while(current!=nullptr){
            if(current->is_free && current->size >= target_size){
                if(!best_buddy || current->size < best_buddy->size){
                    best_buddy = current;
                }
            }
            current = current->next;
        }
        if(best_buddy){
            // 2. Split the block until it is the perfect power-of-two size
            while(best_buddy->size > target_size){
                size_t new_size = best_buddy->size / 2;

                //Create the buddy block
                MemoryBlock* buddy = new MemoryBlock(
                    best_buddy->start_address + new_size,
                    new_size,
                    true, -1
                );

                // Insert into the linked list
                buddy->next = best_buddy->next;
                buddy->prev = best_buddy;
                if(best_buddy->next) best_buddy->next->prev = buddy;
                best_buddy->next = buddy;

                best_buddy->size = new_size;
                // best_buddy remians the one we are looking for further splitting
            }

            selected_block = best_buddy;
        }
    }

    //Now after we find a block, using the split logic to split the memory to use only the required amount of memory
    if(selected_block){
        if(current_strategy == "Buddy"){
            size_t actual_size = selected_block->size;
            if(actual_size > request_size){
                internal_fragmentation += (actual_size - request_size);
            }
        }
        if(current_strategy!="Buddy" && selected_block->size > request_size){
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

        successful_allocs++;
        return (long long) selected_block->start_address;
    }

    failed_allocs++;
    return -1;
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
            //Seperate if condition for buddy 
            bool merged = true;
            while(merged){
                merged = false;

                //Buddy specific coalescing
                if(current_strategy == "Buddy"){
                    // A block's buddy is found by xoring the address with the size
                    size_t buddy_addr = current->start_address ^ current->size;

                    if(current->next && current->next->is_free && 
                       current->next->start_address == buddy_addr &&
                       current->next->size == current->size){
                        
                        MemoryBlock* temp = current->next;
                        current->size += temp->size;
                        current->next = temp->next;
                        if(temp->next) temp->next->prev = current;
                        delete temp;
                        merged = true;
                    }
                    else if(current->prev && current->prev->is_free &&
                            current->prev->start_address == buddy_addr && 
                            current->prev->size == current->size){
                        
                        MemoryBlock* prev_block = current->prev;
                        prev_block->size += current->size;
                        prev_block->next = current->next;
                        if(current->next) current->next->prev = prev_block;
                        MemoryBlock* to_delete = current;
                        current = prev_block;
                        delete to_delete;
                        merged = true;
                    }
                }
                else{
                    // If not buddy
                    //1. Checks if the next block is also free
                    if(current->next!=nullptr && current->next->is_free){
                        MemoryBlock* temp = current->next;
                        current->size+=temp->size;
                        current->next = temp->next;
                        if(temp->next!=nullptr){
                            temp->next->prev = current;
                        }
                        delete temp;
                        merged = true;
                    }

                    //2. checks if previous block is also free
                    if(current->prev!=nullptr && current->prev->is_free){
                        MemoryBlock* prev_block = current->prev;
                        prev_block->size+=current->size; 
                        prev_block->next = current->next;
                        if(current->next != nullptr){
                            current->next->prev = prev_block;
                        }
                        MemoryBlock* to_delete = current;
                        current = prev_block;
                        delete to_delete;
                        merged = true;
                    }
                }
            }

            std::cout << "Process " << process_id << " deallocated and memory coalesced.\n";
        }
        current = current->next;
    }

    if(found){
        std::cout << "Process " << process_id << " virtual memory deallocated.\n";

        int freed_frames = 0;
        for(size_t i = 0; i<total_frames; i++){
            if(frame_table[i] == process_id){
                frame_table[i] = -1;
                freed_frames++;
            }
        }
        std::cout << " -> Released " << freed_frames << " physical frames.\n";

        if(process_page_tables.find(process_id) != process_page_tables.end()){
            delete process_page_tables[process_id];
            process_page_tables.erase(process_id);
        }
    } else {
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

    // Success rate 
    double success_rate = (total_allocs == 0) ? 0.0 :
                          ((double)successful_allocs / total_allocs) * 100.0;

    int occupied_frames = 0;
    for(int i=0; i<(int)total_frames; i++){
        if(frame_table[i] != -1) occupied_frames++;
    }
    double phys_utilization = (static_cast<double>(occupied_frames) / total_frames) * 100.0;

    std::cout << "\n========== MEMORY STATISTICS ==========\n";
    std::cout << "Total Memory:    " << total_size << " bytes\n";
    std::cout << "Used Memory:     " << used_memory << " bytes\n";
    std::cout << "Free Memory:     " << free_memory << " bytes\n";
    std::cout << "Utilization (Virtual): " << std::fixed << std::setprecision(2) << utilization << "% (Overcommitment)\n";
    std::cout << "Utilization (Physical):" << phys_utilization << "% (" << occupied_frames << "/" << total_frames << " frames)\n";
    std::cout << "Internal Fragmentation: " << internal_fragmentation << " bytes\n";
    std::cout << "Allocation Succes Rate: " << success_rate << "%\n";
    std::cout << "External Fragmentation:   " << fragmentation << "%\n";
    std::cout << "Free Block Count: " << free_block_count << "\n";
    std::cout << "\nL1 "; l1_cache->display_stats();
    std::cout << "L2 "; l2_cache->display_stats();
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
