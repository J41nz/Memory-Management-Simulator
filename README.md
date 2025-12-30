# C++ Memory Management Simulator

A comprehensive simulation of an Operating System's Memory Management Unit (MMU). This project demonstrates Virtual Memory allocation, Demand Paging, Page Replacement (FIFO), and a two-level Hardware Cache hierarchy, different allocation strategies (First Fit, Best Fit, Worst Fit, Buddy allocation).

## Project Structure
* `src/`: Implementation files (.cpp)
* `include/`: Header files (.hpp)
* `tests/`: Automated test scripts
* `main.cpp`: CLI entry point

## How to Build and Run

### Prerequisites 
* G++ compiler (supporting C++17)
* Make

### Build
Run the following command in the root directory:
```bash
make
```

### Run
Start the interactive simulator:
```bash
./memsim
```

### Run Automated Tests
To run the provided stress test scenarios (ensure to make the script executable by running: `chmod +x tests/run_tests.sh`):
```bash
./tests/run_tests.sh
```

## Features Implemented 
1. **Memory Allocation Strategies**: First Fit, Best Fit, Worst Fit, and Buddy System.
2. **Virtual Memory**: Per-process Page Tables mapping Virtual Pages to Physical Frames.
3. **Demand Paging**: Lazy loading of pages (Page Fault handling).
4. **Page Replacement**: FIFO eviction policy when Physical RAM is full.
5. **Cache Hierarchy**:
*   L1 Cache: 128B, 2-way Set Associative
*   L2 Cache: 512B, 4-way Set Associative
6. Deallocation: Proper cleanup of Virtual Blocks and Physical Frames (preventing leaks).
