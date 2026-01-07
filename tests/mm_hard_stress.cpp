#include "Allocator/MemoryManager.h"
#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;

int main() {

    MemoryManager mm(128, Strategy::first_fit);

    vector<int> ids;

    for (int step = 0; step < 5000; step++) {

        int op = rand() % 2;

        // allocate
        if (op == 0 || ids.empty()) {
            int size = (rand() % 20) + 1;
            int id = mm.mem_malloc(size);
            ids.push_back(id);
        }

        // free random block
        else {
            int idx = rand() % ids.size();
            if (ids[idx] != -1)
                mm.deallocate(ids[idx]);
            ids[idx] = -1;
        }
    }

    mm.dump();
    mm.print_metrics();
}
