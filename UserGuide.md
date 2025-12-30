# User Guide

## Interactive Shell Commands

The simulator uses a CLI interface. Below are the valid commands.

### 1. System Initialization
* **Command:** `init <size_in_bytes>`
* **Example:** `init 1024`
* **Description** Resets the simulation. Clears all memory, caches, and page tables. Sets Physical RAM size.

### 2. Strategy Selection
* **Command:** `strategy <name>`
* **Options:** `first_fit`, `best_fit`, `worst_fit`, `buddy`
* **Example:** `strategy best_fit`

### 3.Memory Allocation (Virtual)
* **Command:** `malloc <size> <pid>`
* **Example:** `malloc 512 1` (Allocates 512 bytes for Process ID 1)
* **Note:** This allocates *Virtual Memory*. Physical RAM is not touched until `access` is called (Lazy Allocation).

### 4. Memory Access (The CPU)
* **Command:** `access <pid> <virtual_address>`
* **Format:** Address **MUST** ve in Hexadecimal (e.g., `0x10`, `0x200`).
* **Example:** `access 1 0x0`
* **Behaviour:**
    1. Triggers Virtual -> Physical translation.
    2. Handles Page Faults (loads from Disk if needed).
    3. Checks L1/L2 Caches.

### 5. Memory Deallocation
* **Command:** `free <pid>`
* **Example:** `free 1`
* **Behaviour:** Frees Virtual Blocks, releases Physical Frames, and flushes Page Tables.

### 6. Diagnostics
* `stats`: View RAM utilization, fragmentation, and Cache Hit/Miss rates.
* `dump`: View the Virtual Memory linked list map.
* `exit`: Clost the simulator.

## Example Workflow
```text
> init 1024 
> malloc 256 1
> access 1 0x10
> stats 
```
