# Design Document: Memory Management Simulator

## 1. System Overview
The system simulates the interaction between the CPU, MMU (Memory Management Unit), and Physical RAM. It seperates the concept of **Virtual Address Space** (what the process sees) from **Physical Address Space** (what actually exists in RAM).

## 2. Memory Layout

### Virtual Memory (The "Heap")
* Managed via a **Doubly Linked List** of `MemoryBlock` structures.
* Represents the infinite address space available to processes.
* **Fragmentation:** Handled via Coalescing (merging adjacent free blocks) and Buddy System merging.

### Physical Memory (RAM)
* Modeled as a fixed array of **Frames** (`frame_table`).
* Size: Configurable (default 1024 Bytes).
* **Page Size:** 256 Bytes.
* **Tracking:** Each entry in `frame_table` stores the `PID` (Process ID) owning that frame, or `-1` if free.

## 3. Address Translation Flow
When the CPU requests a Virtual Address (`v_addr`):

1. **Identity Page:** `Page_Num = v_addr / Page_Size`.
2. **Check Page Table:** Look up `Page_Num` in the process's `PageTable`.
3. **TLB/Page Walk:** 
   * **Hit:** Return `Frame_Num`.
   * **Miss (Page Fault):**
      1. Find a free frame in RAM.
      2. **If RAM Full (Eviction):** Use FIFO queue to identify the oldest frame. Evict it (invalidate owner's Page Table entry) and claim the frame.
      3. Update Page Table with new mapping.
      4. **Physical Address:** `p_addr = (Frame_Num * Page_Size) + Offset`.
4. **Physical Address:** `p_addr = (Frame_Num * Page_Size) + Offset`.

## 4. Cache Hierarchy Design
The system implements a **Physical Cache**.
* **L1 Cache:** 128 Bytes, 16B Block, 2-Way Set Associative.
* **L2 Cache:** 512 Bytes, 16B Block, 4-Way Set Associative.
* **Policy:** Write-Allocate, standard FIFO replacement within sets.

**Data Flow:**
CPU -> MMU (Translation) -> L1 Cache -> L2 Cache -> Main Memory (RAM)

## 5. Allocation Algorithms 
* **First Fit:** Scans list, picks first sufficient block. Fast but high fragmentation.
* **Best Fit:** Picks largest block. Reduces small external fragments.
* **Buddy System:** Splits memory blocks into powers of two. Highly efficient coalescing using XOR buddy calculation.

## 6. Limitations
* Single-threaded simulation (no race conditions).
* Disk I/O is simulated via sleep delays, not actual file storage.
* Page Tables are single-level (not multi-level), suitable for the small simulation size.


