#include "./include/MemoryManager.hpp"
#include <iostream>
#include <iomanip>
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
            std::cout << "  access <address>    - Checks for an address in L1 and L2 Caches\n";
            std::cout << "  vaccess <pid> <virtual_address> - returns physical address from virtual address of a process\n"; 
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
                auto addr = memSim->allocate(size, pid);
                if(addr==-1)
                    std::cout << "Error: Not enough memory!\n";
                else{
                    std::cout << "Allocated block id=" << pid << " at address=0x"
                              << std::hex << std::setw(4) << std::setfill('0') << addr << std::dec << "\n";
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
        else if(command == "access"){
            size_t addr;
            if(ss >> addr){
                memSim->access_memory(addr);
            }
            std::cout << std::dec;
        }
        else if(command == "vaccess"){
            int pid;
            size_t v_addr;
            if(ss >> pid >> v_addr){
                std::cout << "Translating Virtual Address 0x" << std::hex << v_addr << " for PID " << pid << "...\n";
                size_t p_addr = memSim->virtual_to_physical(pid, v_addr);
                std::cout << "Physical Address: 0x" << std::hex << p_addr << std::dec << "\n";

                // Now pass physical address to the Cache
                memSim->access_memory(p_addr);
            }
        }
        else{
            std::cout << "Unknown command. Type 'help'.\n";
        }
    }
    delete memSim;
    return 0;
}
