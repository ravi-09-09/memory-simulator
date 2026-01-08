# Memory Management Simulator 
## overview
This project implements a memory management simulator that models how an operating system manages memory.

It focuses on:
dynamic memory allocation
multi-level CPU caches
virtual memory and paging

The goal is educational, not to build a real OS kernel.
All behavior is simulated using C++ data structures and algorithms.

## System Architecture 
Virtual Address
      ↓
Virtual Memory (page table + paging)
      ↓
Physical Address
      ↓
Cache Hierarchy (L1 → L2)
      ↓
Memory Manager (physical memory)

## Physical memory & Allocator
4.1 Memory Layout

Physical memory is modeled as a contiguous block.

The simulator supports dynamic allocation using:
First Fit
Best Fit
Worst Fit

Each allocation keeps track of:
start address
size
whether block is free or allocated

### Deallocation & Coalescing

When memory is freed:
the block is marked free
adjacent free blocks are merged (coalescing)
fragmentation metrics are updated

### Metrics

The allocator reports:

Internal fragmentation

External fragmentation

Memory utilization

Allocation success vs failure count

## Cache Simulation
The simulator implements a two-level unified cache (L1 and L2).
Both levels cache data for both reads and writes.

All cache operations happen after address translation, so caches always work on physical addresses.
### Cache Organization

Each cache level is defined by:
| Parameter     | Meaning                            |
| ------------- | ---------------------------------- |
| cache_size    | number of cache *lines*            |
| block_size    | number of integers per cache block |
| associativity | number of lines per set            |
| type          | direct-mapped or set-associative   |
| replacement   | FIFO or LRU                        |

### Address Breakdown
+-----------+---------+---------+
|   TAG     |  INDEX  | OFFSET |
+-----------+---------+---------+
offset → position inside the block
index → which cache set to look in
tag → identifies the block stored there

### Replacement policies
The simulator supports two policies:
1) FIFO (First-In, First-Out)
The oldest inserted block in the set is replaced first.
A queue is maintained per set.

2) LRU (Least Recently Used)

Each line stores a timestamp.
The one not accessed for the longest time is replaced.

### Cache statistics
for each level the simulator reports 
 1) hits 
 2) misses

## Virtual memory 
The simulator implements a paged virtual memory system sitting above the cache and physical memory.
Every memory access follows:
Virtual Address
   ↓
Page Table + Page Fault Handling
   ↓
Physical Address
   ↓
Cache (L1 → L2)
   ↓
Physical Memory

The system supports:
paging
FIFO and LRU page replacement
dirty pages
eviction handling
cache invalidation when frames change

### Virtual Address Space and Ram
The address space is divided into:
num_pages  = ceil(virtual_size / page_size)
num_frames = ram_size / page_size

Two main storage areas exist:
| Storage      | Meaning                                  |
| ------------ | ---------------------------------------- |
| disk         | backing store (simulated disk)           |
| frames (RAM) | physical frames managed by MemoryManager |

The disk holds all virtual memory contents initially
### Page table structure
| Field | Meaning                               |
| ----- | ------------------------------------- |
| valid | whether the page is currently in RAM  |
| frame | which frame it is mapped to           |
| dirty | whether data in RAM differs from disk |

The simulator also tracks:
last_use[page] — used for LRU
fifo queue — used for FIFO

### page fault handling
When a page is accessed but not in RAM:

1) Page fault counter increments
2) Look for a free frame

If a free frame exists → load page there.
If no free frame exists → choose victim frame:
FIFO → evict oldest frame
LRU → evict page not used for longest time

### dirty page handling 
If the victim page is dirty:
RAM → DISK
The data in physical memory is written back to disk before eviction.
Then:
invalidate victim page
mark frame free
remove mapping
### Cache Invalidation 
when a page is invicted the cache may contain the old data so before removing the frame , the 
simulator calls 
invalidate_range(frame_start, frame_end)

This removes cache entries overlapping that frame.
This keeps cache + memory consistent.

### statistics
The simulator reports 
Pages
Frames
Page Faults
Page Hits
Evictions

These metrics help analyze:
locality of reference
impact of page size
aggressiveness of replacement
system stress behavior

## Features Implemented 
1) Allocate memory like malloc()
    int id = mm.mem_malloc(20);
    The simulator:
     returns a block id and reserves that region inside physical memory
     and prints if allocation fails

     You can later free it:
     mm.deallocate(id);
    And see memory layout:
    mm.dump();

2) Read and Write in Allocated Blocks
You can write:
mm.write(id, 3, 100);
You can read back:
int x;
mm.read(id, 3, x);

3) Direct physical memory access
mm.read_physical(address, value);
mm.write_physical(address, value);
They work on actual physical addresses.
Used internally by cache + virtual memory.

4) View memory Statistics 
    mm.print_metrics();
5) Simulate CPU cache access 
To read from cache:
cache.read(address);
To write:
cache.write(address, value);

The cache automatically:
checks L1
checks L2
loads from memory if needed
applies FIFO / LRU

To view cache statistics
cache.stats();

6) Work with virtual Memory (virtual - > physical translation)
read using virtual address
vm.read(25);
write:
vm.write(25, 300);

The simulator automatically:
translates address
triggers page faults
replaces pages if RAM is full
maintains disk copy
updates cache
To see VM statistics:
vm.stats();

## Programs for terminal 
### for building
make
### Run main demo
./simulator
### Run Paging example 
./vm_normal
### Run heavy Cache test
./cache_hard
### Run paging stress test 
./vm_hard
All programs print results automatically — no extra setup required.
## Video Demo
https://drive.google.com/file/d/1fYDRHj_w-l2nLzhAN5qX13wcweJGB0yB/view?usp=sharing
