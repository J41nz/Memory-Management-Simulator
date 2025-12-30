#include "./include/MemoryManager.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

int main(){
    std::cout << "==========================================\n";
    std::cout << "      Memory Management Simulator\n";
    std::cout << "==========================================\n";
    std::cout << "Type 'help' for a list of commands.\n";

    // Initialized with 1024 Bytes of Physical RAM
    MemoryManager* memSim = new MemoryManager(1024);
    std::string line;
    
    while(true){
        std::cout << "\n> ";
        if(!std::getline(std::cin, line)) break;

        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if(command == "exit"){
            break;
        }
        else if(command == "help"){
            std::cout << "Commands: \n";
            std::cout << "  init <size>          - Reinitialize physical memory size\n";
            std::cout << "  malloc <size> <pid>  - Allocate Virtual Memory for a process\n";
            std::cout << "  free <pid>           - Deallocate memory for a process\n";
            std::cout << "  strategy <name>      - Set strategy (first_fit, best_fit, worst_fit, buddy)\n";
            std::cout << "  access <pid> <addr>  - CPU accesses Virtual Address (Triggers VM translation)\n";
            std::cout << "  dump                 - Show Memory Block Map (Virtual)\n";
            std::cout << "  stats                - Show Physical RAM stats & Cache Hit Rates\n";
        }
        else if(command == "init"){
            size_t size;
            ss >> size;
            delete memSim;
            memSim = new MemoryManager(size);
            std::cout << "System Re-initialized with " << size << " bytes of Physical RAM.\n";
        }
        else if(command == "malloc"){
            size_t size;
            int pid;
            if(ss >> size >> pid){
                auto addr = memSim->allocate(size, pid);
                if(addr == -1)
                    std::cout << "Error: Allocation Failed (Virtual Address Space Full)\n";
                else{
                    std::cout << "Allocated Virtual Block for PID " << pid << " @ 0x" 
                              << std::hex << addr << std::dec << "\n";
                }
            }
        }
        else if(command == "free"){
            int pid;
            if (ss >> pid) memSim->deallocate(pid);
        }
        else if(command == "strategy"){
            std::string strat;
            ss >> strat;
            if(strat == "first_fit") memSim->set_strategy("First Fit");
            else if(strat == "best_fit") memSim->set_strategy("Best Fit");
            else if(strat == "worst_fit") memSim->set_strategy("Worst Fit");
            else if(strat == "buddy") memSim->set_strategy("Buddy");
            std::cout << "Allocation Strategy set to: " << strat << "\n";
        }
        else if(command == "dump"){
            memSim->dump_memory();
        }
        else if(command == "stats"){
            memSim->display_stats();
        }

        else if(command == "access"){
            int pid;
            size_t v_addr;
            if(ss >> pid >> std::hex >> v_addr){
                memSim->access_memory(v_addr, pid);
            } else {
                std::cout << "Usage: access <pid> <virtual_address>\n";
            }
        }
        else{
            std::cout << "Unknown command. Type 'help'.\n";
        }
    }
    delete memSim;
    return 0;
}