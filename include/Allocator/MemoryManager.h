#pragma once
#include <bits/stdc++.h>
using namespace std;

enum class Strategy {
    first_fit, best_fit, worst_fit
};

struct block {
    int start, size;
    bool free;
    int id, requested;

    block(int start, int size, bool free, int id);
};

class MemoryManager {
private:
    int mem_size, next_id;
    list<block> blocks;
    Strategy strategy;
    vector<int> memory;

    unordered_map<int, list<block>::iterator> id_map;

public:
    int alloc_success, alloc_fail;

    MemoryManager(int mem_size, Strategy strategy);

    int mem_malloc(int alloc_size);
    void deallocate(int curr_id);

    int get_physical_address(int block_id, int offset);

    bool write(int block_id, int offset, int val);
    bool read(int block_id, int offset, int &value);

    bool read_physical(int address, int &value);
    bool write_physical(int address, int value);

    void dump();
    void print_metrics();
};
