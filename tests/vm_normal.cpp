#include <iostream>
#include <cstdlib>

#include "Allocator/MemoryManager.h"
#include "Cache/Cache.h"
#include "VirtualMemory/VirtualMemory.h"

using namespace std;

int main() {

    srand(time(nullptr));

    // physical memory
    MemoryManager mm(256, Strategy::first_fit);

    // cache system
    TwoLevelUnifiedCache cache(
        CacheType::SET_ASSOC, Replacement::LRU,
        8, 2,     // L1
        CacheType::SET_ASSOC, Replacement::LRU,
        16, 4,    // L2
        4,
        mm
    );

    // virtual memory
    VirtualMemory vm(
        256,   // virtual space
        128,   // physical RAM
        8,     // page size
        cache,
        mm,
        PagePolicy::LRU
    );

    cout << "\n--- VM NORMAL TEST ---\n";

    // 1️⃣ simple sequential touches
    for (int i = 0; i < 80; i++)
        vm.read(i);

    // 2️⃣ write some values
    for (int i = 0; i < 40; i++)
        vm.write(30 + i, i * 10);

    // 3️⃣ read back written values
    bool ok = true;
    for (int i = 0; i < 40; i++) {
        int val = vm.read(30 + i);
        if (val != i * 10) {
            cout << "Mismatch at " << (30 + i)
                 << " expected " << (i * 10)
                 << " got " << val << "\n";
            ok = false;
        }
    }

    if (ok)
        cout << "Read-back correctness passed.\n";

    vm.stats();
    cache.stats();
}
