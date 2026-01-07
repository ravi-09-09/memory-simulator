#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "Allocator/MemoryManager.h"
using namespace std;

int main() {
    srand(time(nullptr));

    MemoryManager mm(128, Strategy::first_fit);

    vector<int> ids;

    cout << "\n--- ALLOC LOOP ---\n";

    // allocate many random sizes
    for (int i = 0; i < 20; i++) {
        int size = (rand() % 12) + 1;
        int id = mm.mem_malloc(size);
        ids.push_back(id);
        cout << "alloc(" << size << ") -> " << id << endl;
    }

    cout << "\n--- RANDOM FREE ---\n";

    // randomly free
    for (int id : ids) {
        if (rand() % 2 && id != -1) {
            cout << "free(" << id << ")\n";
            mm.deallocate(id);
        }
    }

    cout << "\n--- ALLOC AGAIN ---\n";
    mm.mem_malloc(10);

    mm.dump();
    mm.print_metrics();
}
