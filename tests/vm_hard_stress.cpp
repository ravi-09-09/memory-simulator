/** vm_hard_stress.cpp **/

#include "Allocator/MemoryManager.h"
#include "Cache/Cache.h"
#include "VirtualMemory/VirtualMemory.h"

#include <bits/stdc++.h>
using namespace std;

int main() {

    const int VIRTUAL = 4096;     // virtual address space
    const int RAM     = 1024;     // physical RAM
    const int PAGE    = 64;       // page size
    const int BLOCK   = 16;       // cache block size

    // ----- MEMORY MANAGER -----
    MemoryManager mm(RAM, Strategy::first_fit);

    // ----- TWO LEVEL CACHE -----
    TwoLevelUnifiedCache cache(
        CacheType::SET_ASSOC, Replacement::LRU,
        256, 2,                    // L1 size, assoc

        CacheType::SET_ASSOC, Replacement::LRU,
        512, 4,                    // L2 size, assoc

        BLOCK,
        mm
    );

    // ----- VIRTUAL MEMORY -----
    VirtualMemory vm(
        VIRTUAL,
        RAM,
        PAGE,
        cache,
        mm,
        PagePolicy::LRU
    );

    cout << "\n--- VM HARD STRESS TEST ---\n";

    // random stress workload
    mt19937 rng(12345);
    uniform_int_distribution<int> addr(0, VIRTUAL - 1);
    uniform_int_distribution<int> val(1, 1000);

    // lots of writes
    for (int i = 0; i < 5000; i++) {
        int a = addr(rng);
        vm.write(a, val(rng));
    }

    // mixed read/write workload
    for (int i = 0; i < 5000; i++) {
        int a = addr(rng);
        if (i % 3 == 0)
            vm.write(a, val(rng));
        else
            vm.read(a);
    }

    // read back some range to sanity-check
    bool ok = true;
    for (int i = 0; i < 256; i++)
        vm.read(i);

    cout << "\nHard stress complete.\n";

    vm.stats();
    cache.stats();
}
