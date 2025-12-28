#include "Cache.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

Cache::Cache(size_t cache_size, size_t block_sz)
    : size(cache_size), block_size(block_sz){
        num_lines = size / block_size;
}

bool Cache::access(size_t address){
    size_t tag = address / block_size; // The "ID" of the memory block

    // 1. Check for a HIT: Is this tag already in our FIFO queue?
    auto it = std::find(fifo_queue.begin(), fifo_queue.end(), tag);

    if(it!=fifo_queue.end()){
        hits++; return true;
    }

    // 2. If it's a miss
    misses++;

    // 3. Check if the cache is full
    if(fifo_queue.size()>=num_lines){
        //Cache full: fifo replacement policy in action
        //remove the oldest element (the one at the front of the queue)
        fifo_queue.pop_front();
    }

    // 4. Add the new block to the back of the queue (it's the newest)
    fifo_queue.push_back(tag);

    return false; //MISS
}

void Cache::display_stats(){
    double total_accesses = hits+misses;
    double hit_ratio = (total_accesses==0) ? 0 : (double)hits / (total_accesses) * 100;
    std::cout << "Cache Stats -> Hits: " << hits 
              << " | Misses: " << misses
              << " | Hit Ratio: " << std::fixed << std::setprecision(2)
              << hit_ratio << "%\n";
}