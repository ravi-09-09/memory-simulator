#include <bits/stdc++.h>

#include "Allocator/MemoryManager.h"
#include "Cache/Cache.h"
#include "VirtualMemory/VirtualMemory.h"

using namespace std;

int main(){
    cout << "\n===== MEMORY MANAGER — DEMO =====\n\n";

    // 64 bytes — First Fit
    MemoryManager mm(64, Strategy::first_fit);

    cout << "Initial layout:\n";
    mm.dump();

    // ----------- ALLOCATIONS -----------
    cout << "Allocating A(10), B(12), C(14)...\n";
    int A = mm.mem_malloc(10);
    int B = mm.mem_malloc(12);
    int C = mm.mem_malloc(14);

    mm.dump();
    mm.print_metrics();

    // ----------- DEALLOCATIONS (coalescing example) -----------
    cout << "Deallocating B (12 bytes)...\n";
    mm.deallocate(B);
    mm.dump();

    cout << "Deallocating C (14 bytes) — these two free blocks should coalesce.\n";
    mm.deallocate(C);
    mm.dump();          // <— here we SEE the merge happen
    mm.print_metrics();

    // ----------- NEW ALLOCATION (after coalescing) -----------
    cout << "Allocating D(20) — should fit in merged free region.\n";
    int D = mm.mem_malloc(20);
    mm.dump();
    mm.print_metrics();

    // ----------- READ / WRITE (logical) -----------
    cout << "Writing value 55 at block A, offset 3...\n";
    mm.write(A, 3, 55);

    int value_block = 0;
    mm.read(A, 3, value_block);
    cout << "Read from block A, offset 3 -> " << value_block << "\n\n";

    // ----------- PHYSICAL ADDRESS DEMO -----------
    cout << "Getting physical address for (A, offset 3)...\n";
    int phys = mm.get_physical_address(A, 3);
    cout << "Physical address = " << phys << "\n";

    cout << "Writing 77 directly to physical address...\n";
    mm.write_physical(phys, 77);

    int value_phys = 0;
    mm.read_physical(phys, value_phys);
    cout << "Reading from physical address -> " << value_phys << "\n\n";

    cout << "Final metrics:\n";
    mm.print_metrics();

    cout << "===== DEMO COMPLETE =====\n";


    // for cache 
    cout << "\n===== CACHE — FULL PATTERN DEMO =====\n\n";

    MemoryManager mm_cache(128, Strategy::first_fit);

    cout << "Initializing physical memory with 0..127\n";
    for (int i = 0; i < 128; i++)
        mm_cache.write_physical(i, i);


    TwoLevelUnifiedCache cache(
        CacheType::SET_ASSOC, Replacement::LRU,
        16, 2,              // L1 small (forces eviction)
        CacheType::SET_ASSOC, Replacement::LRU,
        64, 2,              // L2 bigger (keeps blocks longer)
        4,
        mm_cache
    );

    // 1️ COLD MISS — both levels miss
    cout << "\n[1] Read 10  → expect: L1 miss, L2 miss\n";
    int x = cache.read(10);
    cout << "Value = " << x << "\n";

    //  L1 HIT
    cout << "\n[2] Read 10 again → expect: L1 hit\n";
    cache.read(10);

    // Fill L1 so that block of 10 gets evicted from L1
    cout << "\n[3] Filling L1 with other blocks to evict block [8–11]…\n";

    cache.read(20);
    cache.read(30);
    cache.read(40);
    cache.read(50);

    cout << "Likely evicted from L1, still present in L2.\n";

    // L2 HIT (L1 miss, L2 hit)
    cout << "\n[4] Read 10 again → expect: L1 miss, L2 hit\n";
    cache.read(10);
    cache.write(10,11);
    // Now L1 HIT again
    cout << "\n[5] Read 10 again → expect: L1 hit\n";
    cache.read(10);

    cout << "\n===== CACHE STATS =====\n";
    cache.stats();

    cout << "\n===== DEMO COMPLETE =====\n";


    // virtual memory 
    cout << "\n===== VIRTUAL MEMORY — DEMO =====\n\n";

    // Physical RAM backing VM
    MemoryManager mm_vm(256, Strategy::first_fit);

    // Separate cache for VM (so stats are clean)
    TwoLevelUnifiedCache vm_cache(
        CacheType::SET_ASSOC, Replacement::LRU,
        16, 2,          // L1
        CacheType::SET_ASSOC, Replacement::FIFO,
        64, 4,          // L2
        4,
        mm_vm
    );

    VirtualMemory vm(
        128,        // virtual address size
        64,         // RAM size
        4,          // page size
        vm_cache,
        mm_vm,
        PagePolicy::FIFO
    );

    cout << "Writing to virtual addresses 5, 6, 25...\n";
    vm.write(5, 100);
    vm.write(6, 200);
    vm.write(25, 300);

    cout << "Reading back values...\n";
    cout << "v[5]  = " << vm.read(5)  << "\n";
    cout << "v[6]  = " << vm.read(6)  << "\n";
    cout << "v[25] = " << vm.read(25) << "\n";

    cout << "\n===== VM STATS =====\n";
    vm.stats();

    cout << "\n===== CACHE (USED BY VM) STATS =====\n";
    vm_cache.stats();

    cout << "\n===== MEMORY METRICS (RAM) =====\n";
    mm_vm.print_metrics();

    cout << "\n===== DEMO COMPLETE =====\n";

    return 0 ; 
}