#include <iostream>
#include <cstdlib>
#include <ctime>

#include "Allocator/MemoryManager.h"
#include "Cache/Cache.h"

using namespace std;

void separator(const string &name) {
    cout << "\n================ " << name << " ================\n";
}

int main() {

    srand(time(nullptr));

    MemoryManager mem(512, Strategy::first_fit);

    TwoLevelUnifiedCache cache(
        CacheType::SET_ASSOC, Replacement::LRU,
        16, 2,     // L1
        CacheType::SET_ASSOC, Replacement::LRU,
        32, 4,     // L2
        8,
        mem
    );

    // preload memory
    for (int i = 0; i < 400; i++)
        mem.write_physical(i, i);

    // 1️⃣ Sequential access (should build up high L1 hit rate)
    separator("SEQUENTIAL ACCESS");

    for (int i = 0; i < 200; i++)
        cache.read(i);

    cache.stats();

    // 2️⃣ Conflict pattern (force same index repeatedly)
    separator("CONFLICT / THRASH PATTERN");

    for (int i = 0; i < 200; i++) {
        int a = 0;
        int b = 32;
        int c = 64;

        cache.read(a + (i % 8));
        cache.read(b + (i % 8));
        cache.read(c + (i % 8));
    }

    cache.stats();

    // 3️⃣ Random mix
    separator("RANDOM READ + WRITE");

    for (int i = 0; i < 2000; i++) {
        int addr = rand() % 300;
        if (rand() % 2)
            cache.read(addr);
        else
            cache.write(addr, rand() % 10000);
    }

    cache.stats();

    // 4️⃣ Read-after-write correctness check
    separator("READ AFTER WRITE CHECK");

    bool ok = true;

    for (int i = 0; i < 100; i++) {
        int addr = 50 + (i % 40);
        int val = i * 7;
        cache.write(addr, val);

        int r = cache.read(addr);
        if (r != val) {
            cout << "Mismatch at " << addr << " expected " << val << " got " << r << "\n";
            ok = false;
        }
    }

    if (ok) cout << "All read-after-write checks passed.\n";

    cache.stats();

    // 5️⃣ Working-set overflow
    separator("WORKING SET OVERFLOW");

    for (int i = 0; i < 3000; i++)
        cache.read(rand() % 500);

    cache.stats();

    return 0;
}
