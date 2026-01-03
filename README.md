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

## Demo Video


https://github.com/user-attachments/assets/da7a43d0-8185-4d98-842b-4bd0b955d2d5


## Technical Disclaimer & Simulation Scope
**Note on OS Fidelity: This project is a functional simulation designed to demonstrate the mathematical and logical principles of memory management (e.g., Buddy System, Page Tables, and Set-Associative Caching).**
### Key Simplifications:
* Implicit Mapping: Unlike a production OS (like Linux or Windows), which would trigger a Segmentation Fault when accessing unallocated memory, this simulator uses an "Implicit Demand Paging" model. If a CPU request is made for an unmapped page, the system automatically handles it as a Page Fault and maps a physical frame, regardless of whether malloc() was previously called.
* Lack of Protection Bits: To focus on the translation pipeline, we have omitted hardware-level protection bits (Read/Write/Execute).
* Abstraction of Integration: The simulation focuses on the Address Translation Pipeline rather than the complex integration between the Virtual Memory Manager's state and the CPU's instruction set.
