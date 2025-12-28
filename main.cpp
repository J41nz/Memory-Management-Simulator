#include "./include/MemoryManager.hpp"
#include <iostream>
#include <string>
#include <sstream>

int main(){
    std::cout << "Memory Management Simulator\n";
    std::cout << "Type 'help' for a list of commands.\n";

    //Initialize with a default size
    MemoryManager* memSim = new MemoryManager(1024);
    std::string line;
    
    while(true){
        std::cout << ">";
        if(!std::getline(std::cin, line)) break;

        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if(command == "exit"){
            break;
        }
        else if(command == "help"){
            std::cout << "Commands: \n";
            std::cout << "  init <size>         - Reinitialize memory with new size\n";
            std::cout << "  malloc <size> <pid> - Allocate memory for a process\n";
            std::cout << "  free <pid>          - Deallocate memory for a process\n";
            std::cout << "  strategy <name>     - Set strategy (first_fit, best_fit, worst_fit)\n";
            std::cout << "  dump                - Show memory map\n";
            std::cout << "  stats               - Show fragmentation and utilization\n";
        }
        else if(command == "init"){
            size_t size;
            ss >> size;
            delete memSim;
            memSim = new MemoryManager(size);
            std::cout << "Memory initialized to " << size << " bytes.\n";
        }
        else if(command == "malloc"){
            size_t size;
            int pid;
            if(ss >> size >> pid){
                void* addr = memSim->allocate(size, pid);
                if(addr!=nullptr)
                    std::cout << "Allocated at address: " << addr << "\n";
                else
                    std::cout << "Error: Not enough memory!\n";
            }
        }
        else if(command == "free"){
            int pid;
            if (ss >> pid) memSim->deallocate(pid);
        }
        else if(command == "strategy"){
            std::string strat;
            ss >> strat;
            // Map the simple string to formal name 
            if(strat == "first_fit") memSim->set_strategy("First Fit");
            else if(strat == "best_fit") memSim->set_strategy("Best Fit");
            else if(strat == "worst_fit") memSim->set_strategy("Worst Fit");
            std::cout << "Strategy set to " << strat << "\n";
        }
        else if(command == "dump"){
            memSim->dump_memory();
        }
        else if(command == "stats"){
            memSim->display_stats();
        }
        else{
            std::cout << "Unknown command. Type 'help'.\n";
        }
    }
    delete memSim;
    return 0;
}
