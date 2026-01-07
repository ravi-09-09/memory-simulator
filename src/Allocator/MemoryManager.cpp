#include "Allocator/MemoryManager.h"
#include<bits/stdc++.h>
using namespace std ; 
block::block(int start, int size, bool free, int id)
    : start(start), size(size), free(free), id(id), requested(size) {}

MemoryManager::MemoryManager(int mem_size, Strategy strategy)
    : mem_size(mem_size), strategy(strategy) {
    blocks.push_back({0, mem_size, true, -1});
    memory.assign(mem_size, 0);
    next_id = 1;
    alloc_fail = alloc_success = 0;
}

int MemoryManager::mem_malloc(int alloc_size) {

    auto chosen = blocks.end();

    if (strategy == Strategy::first_fit) {
        for (auto it = blocks.begin(); it != blocks.end(); it++) {
            if (it->free && it->size >= alloc_size) {
                chosen = it;
                break;
            }
        }
    }

    if (strategy == Strategy::best_fit) {
        int rem = INT_MAX;
        for (auto it = blocks.begin(); it != blocks.end(); it++) {
            if (it->free && it->size >= alloc_size &&
                (it->size - alloc_size) < rem)
            {
                rem = it->size - alloc_size;
                chosen = it;
            }
        }
    }

    if (strategy == Strategy::worst_fit) {
        int rem = -1;
        for (auto it = blocks.begin(); it != blocks.end(); it++) {
            if (it->free && it->size >= alloc_size &&
                (it->size - alloc_size) > rem)
            {
                rem = it->size - alloc_size;
                chosen = it;
            }
        }
    }

    if (chosen == blocks.end()) {
        alloc_fail++;
        cout << "Allocation failed\n";
        return -1;
    }

    alloc_success++;

    if (chosen->size > alloc_size) {
        block new_block(
            chosen->start + alloc_size,
            chosen->size - alloc_size,
            true,
            -1
        );

        blocks.insert(next(chosen), new_block);
    }

    chosen->size = alloc_size;
    chosen->requested = alloc_size;
    chosen->free = false;
    chosen->id = next_id++;

    id_map[chosen->id] = chosen;
    return chosen->id;
}

void MemoryManager::deallocate(int curr_id) {

    auto it = id_map.find(curr_id);
    if (it == id_map.end()) {
        cout << "Invalid deallocation\n";
        return;
    }

    auto blk = it->second;
    id_map.erase(curr_id);

    blk->free = true;
    blk->id = -1;

    auto next_it = next(blk);
    if (next_it != blocks.end() && next_it->free) {
        blk->size += next_it->size;
        blocks.erase(next_it);
    }

    if (blk != blocks.begin()) {
        auto prev_it = prev(blk);
        if (prev_it->free) {
            prev_it->size += blk->size;
            blocks.erase(blk);
        }
    }
}

int MemoryManager::get_physical_address(int block_id, int offset) {

    auto it = id_map.find(block_id);
    if (it == id_map.end()) {
        cout << "No such block\n";
        return -1;
    }

    auto b = it->second;

    if (offset < 0 || offset >= b->size) {
        cout << "offset out of bound\n";
        return -1;
    }

    return b->start + offset;
}

bool MemoryManager::write(int block_id, int offset, int val) {
    int address = get_physical_address(block_id, offset);
    if (address == -1) return false;

    memory[address] = val;
    return true;
}

bool MemoryManager::read(int block_id, int offset, int &value) {
    int address = get_physical_address(block_id, offset);
    if (address == -1) return false;

    value = memory[address];
    return true;
}

bool MemoryManager::read_physical(int address, int &value) {
    if (address < 0 || address >= mem_size) {
        cout << "physical read out of range\n";
        return false;
    }
    value = memory[address];
    return true;
}

bool MemoryManager::write_physical(int address, int value) {
    if (address < 0 || address >= mem_size) {
        cout << "physical write out of range\n";
        return false;
    }
    memory[address] = value;
    return true;
}

void MemoryManager::dump() {
    cout << "Memory Layout:\n";
    for (auto &b : blocks) {
        cout << "[" << b.start << " - " << b.start + b.size - 1 << "] ";
        if (b.free)
            cout << "Free\n";
        else
            cout << "block_id=" << b.id << "\n";
    }
    cout << endl;
}

void MemoryManager::print_metrics() {

    int tot_free = 0, lar_free = 0, tot_used = 0;
    int internal = 0;

    for (auto &b : blocks) {

        if (b.free) {
            tot_free += b.size;
            lar_free = max(lar_free, b.size);
        } else {
            tot_used += b.size;
            internal += (b.size - b.requested);
        }
    }

    int external = tot_free - lar_free;
    double utilization = (tot_used * 100.0) / mem_size;

    cout << "====== Metrics ======\n";
    cout << "Internal Fragmentation : " << internal << "\n";
    cout << "External Fragmentation : " << external << "\n";
    cout << "Memory Utilization     : " << utilization << "%\n";
    cout << "Alloc Success Count    : " << alloc_success << "\n";
    cout << "Alloc Failure Count    : " << alloc_fail << "\n";
    cout << "=====================\n\n";
}
