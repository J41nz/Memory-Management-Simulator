#ifndef CACHE_HPP
#define CACHE_HPP

#include <vector>
#include <list>
#include <cmath>
#include <iostream>
#include <iomanip>

class Cache{
private:
    struct CacheLine{
        size_t tag;
        bool valid;
    };
    
    // A set contains multiple ways(lines) and a replacement queue
    struct CacheSet{
        std::vector<CacheLine> lines;
        std::list<size_t> fifo_queue; 
    };

    size_t cache_size;
    size_t block_size;
    size_t associativity; // K-way
    size_t num_sets;

    std::vector<CacheSet> sets;

    //Stats
    long long hits = 0;
    long long misses = 0;

public:
    Cache(size_t size, size_t block_sz, size_t assoc);

    bool access(size_t address);
    void display_stats();
};

#endif