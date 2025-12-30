#include "Cache.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

Cache::Cache(size_t size, size_t block_sz, size_t assoc)
    : cache_size(size), block_size(block_sz), associativity(assoc) {
    if (block_sz == 0 || assoc == 0) {
        std::cerr << "Error: Invalid Cache Configuration (Block Size or Assoc is 0)\n";
        num_sets = 1; // Fallback to avoid crash
    } else {
        num_sets = cache_size / (block_size * associativity);
    }
    sets.resize(num_sets);

    //Initialize lines in each set
    for(auto& set : sets){
        set.lines.resize(associativity, {0, false});
    }
}

bool Cache::access(size_t address){
    size_t block_addr = address / block_size;
    size_t set_index = block_addr % num_sets;
    size_t tag = block_addr / num_sets;

    auto& set = sets[set_index];


    // 1. Check for a HIT
    for(size_t i = 0; i < associativity; i++){
        if(set.lines[i].valid && set.lines[i].tag == tag){
            hits++;
            return true;
        }
    }

    misses++; 
    //If queue size < associativity, we just add
    //If queue size == associativity, we pop front (FIFO eviction).

    size_t way_to_use = 0;
    if(set.fifo_queue.size() < associativity){
        //Still have space in this set
        way_to_use = set.fifo_queue.size();
        set.fifo_queue.push_back(way_to_use);
    } else {
        //Set is full -> evict the front member according to the FIFO policy
        way_to_use = set.fifo_queue.front();
        set.fifo_queue.pop_front();
        set.fifo_queue.push_back(way_to_use);
    }

    //Update the hardware line
    set.lines[way_to_use].valid = true;
    set.lines[way_to_use].tag = tag;

    return false;
}

void Cache::display_stats(){
    long long total = hits+misses;
    double ratio = (total == 0) ? 0.0 : (double)hits / total * 100.0;
    std::cout << "Size: " << cache_size << "B | Assoc: " << associativity
              << "-way | Hits: " << hits << " | Misses: " << misses
              << " | Hit Ratio: " << std::fixed << std::setprecision(2) << ratio << "%\n";
}