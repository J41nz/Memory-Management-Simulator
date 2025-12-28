#include "./include/MemoryManager.hpp"
#include <iostream>

int main(){
    //1. initialize a 1024 byte memory pool
    MemoryManager myOS(1000);
    myOS.allocate(200, 1);
    myOS.allocate(200, 2);
    myOS.dump_memory();

    std::cout << "\nFreeing Process 1...\n";
    myOS.deallocate(1);
    myOS.dump_memory();

    std::cout << "\nFreeing Process 2 (Watch coalescing happen!)...\n";
    myOS.deallocate(2);
    myOS.dump_memory();
    
    return 0;
}
