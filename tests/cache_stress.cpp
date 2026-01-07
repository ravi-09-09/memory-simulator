#include <iostream>
#include <cstdlib>

#include "Allocator/MemoryManager.h"
#include "Cache/Cache.h"

using namespace std;

int main() {

    srand(time(nullptr));

    // physical memory backing the cache
    MemoryManager mem(512, Strategy::first_fit);

    // two-level cache
    TwoLevelUnifiedCache cache(
        CacheType::SET_ASSOC, Replacement::LRU,
        16, 2,   // L1: 16 lines, 2-way
        CacheType::SET_ASSOC, Replacement::LRU,
        32, 4,   // L2: 32 lines, 4-way
        8,       // block size
        mem
    );

    // initialize physical memory
    for (int i = 0; i < 300; i++)
        mem.write_physical(i, i);

    cout << "\n--- CACHE STRESS TEST ---\n";

    // mix of reads and writes
    for (int i = 0; i < 3000; i++) {

        int addr = rand() % 300;

        if (rand() % 2) {
            // READ
            cache.read(addr);
        } else {
            // WRITE
            int val = rand() % 1000;
            cache.write(addr, val);
        }
    }

    cache.stats();
}
