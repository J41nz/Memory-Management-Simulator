#include "./include/MemoryManager.hpp"
#include <iostream>

int main(){
    //1. initialize a 1024 byte memory pool
    MemoryManager myOS(1024);

    std::cout << "Initial Memory State: ";
    myOS.dump_memory();

    //2. Simulate some allocations
    std::cout << "Allocating 200 bytes for Process 1...\n";
    myOS.allocate(200, 1);

    std::cout << "Allocating 150 bytes for Process 2...\n";
    myOS.allocate(150, 2);

    //See the results
    myOS.dump_memory();
    
    return 0;
}
