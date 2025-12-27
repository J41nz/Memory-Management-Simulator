#ifndef MEMORY_BLOCK_HPP
#define MEMORY_BLOCK_HPP

#include <iostream>

struct MemoryBlock{
    size_t start_address; // Where the block begins in memory
    size_t size; // How large the block is (in bytes)
    bool is_free; // true if available, false if allocated
    int process_id; // To track which "process" owns this block (-1 if free)

    MemoryBlock* next; // Pointer to the next block in memory
    MemoryBlock* prev; // Pointer to the previous block in memory

    // Constructor to initialize a block easily
    MemoryBlock(size_t addr, size_t sz, bool free = true, int pid = -1)
        : start_address(addr), size(sz), is_free(free), process_id(pid), next(nullptr), prev(nullptr) {}
};

#endif