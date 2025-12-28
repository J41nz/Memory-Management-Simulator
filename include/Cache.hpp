#ifndef CACHE_HPP
#define CACHE_HPP

#include <vector>
#include <unordered_map>
#include <list>

struct CacheLine{
    size_t tag;
    bool valid = false;
};

class Cache{
private:
    size_t size;        // Total bytes in cache
    size_t block_size;  // Size of one cache line
    size_t num_lines;
    std::vector<CacheLine> lines;

    //For FIFO Replacement
    std::list<size_t> fifo_queue;

    int hits = 0;
    int misses = 0;

public:
    Cache(size_t cache_size, size_t block_sz);
    bool access(size_t address); //Returns true if HIT, false if MISS
    void display_stats();
};

#endif