#include "Cache/Cache.h"
#include "Allocator/MemoryManager.h"
#include <bits/stdc++.h>
using namespace std;

/* ===================== CacheLevel ===================== */

CacheLevel::CacheLevel(CacheType type,
                       Replacement policy,
                       int cache_size,
                       int block_size,
                       int associativity)
    : type(type),
      policy(policy),
      cache_size(cache_size),
      block_size(block_size),
      associativity(associativity)
{
    // IMPORTANT: correct number of sets
    num_sets = cache_size / (block_size * associativity);

    sets.resize(num_sets, vector<Line>(associativity));
    fifo.resize(num_sets);

    for (auto &set : sets)
        for (auto &line : set)
            line.block.assign(block_size, 0);
}

int CacheLevel::get_index(int address) {
    return (address / block_size) % num_sets;
}

int CacheLevel::get_tag(int address) {
    return (address / block_size) / num_sets;
}

int CacheLevel::get_offset(int address) {
    return address % block_size;
}

int CacheLevel::choose_victim(int set_index) {

    if (associativity == 1)
        return 0;

    if (policy == Replacement::FIFO) {

        if (fifo[set_index].size() < (size_t)associativity) {
            int way = fifo[set_index].size();
            fifo[set_index].push(way);
            return way;
        }

        int way = fifo[set_index].front();
        fifo[set_index].pop();
        fifo[set_index].push(way);
        return way;
    }

    int best_way = 0;
    int oldest = INT_MAX;

    for (int way = 0; way < associativity; way++) {

        if (!sets[set_index][way].valid)
            return way;

        if (sets[set_index][way].timestamp < oldest) {
            oldest = sets[set_index][way].timestamp;
            best_way = way;
        }
    }

    return best_way;
}

bool CacheLevel::read(int address, int &value) {

    int idx = get_index(address);
    int tag = get_tag(address);
    int off = get_offset(address);

    time_counter++;

    for (auto &line : sets[idx]) {
        if (line.valid && line.tag == tag) {
            hits++;
            line.timestamp = time_counter;
            value = line.block[off];
            return true;
        }
    }

    misses++;
    return false;
}

void CacheLevel::insert_block(int address, const vector<int> &blk) {

    int idx = get_index(address);
    int tag = get_tag(address);

    int victim_way = choose_victim(idx);
    Line &victim = sets[idx][victim_way];

    victim.valid = true;
    victim.tag   = tag;
    victim.dirty = false;
    victim.block = blk;

    time_counter++;
    victim.timestamp = time_counter;
}

bool CacheLevel::write(int address, int value) {

    int idx = get_index(address);
    int tag = get_tag(address);
    int off = get_offset(address);

    time_counter++;

    for (auto &line : sets[idx]) {
        if (line.valid && line.tag == tag) {
            hits++;
            line.timestamp = time_counter;
            line.block[off] = value;
            line.dirty = true;
            return true;
        }
    }

    misses++;
    return false;
}

vector<int> CacheLevel::get_block(int address) {

    int idx = get_index(address);
    int tag = get_tag(address);

    for (auto &line : sets[idx]) {
        if (line.valid && line.tag == tag)
            return line.block;
    }

    return vector<int>(block_size, 0);
}

void CacheLevel::invalidate_range(int start, int end) {

    for (int set = 0; set < num_sets; set++) {
        for (int way = 0; way < associativity; way++) {

            Line &line = sets[set][way];
            if (!line.valid) continue;

            int block_number = line.tag * num_sets + set;
            int base_addr = block_number * block_size;
            int last_addr = base_addr + block_size - 1;

            bool overlaps = !(last_addr < start || base_addr > end);

            if (overlaps) {
                line.valid = false;
                line.dirty = false;
            }
        }
    }
}


/* ===================== TwoLevelUnifiedCache ===================== */

TwoLevelUnifiedCache::TwoLevelUnifiedCache(
    CacheType type1, Replacement p1,
    int l1_lines, int assoc1,

    CacheType type2, Replacement p2,
    int l2_lines, int assoc2,

    int block_size,
    MemoryManager &mm
)
: L1(type1, p1, l1_lines, block_size, assoc1),
  L2(type2, p2, l2_lines, block_size, assoc2),
  mem(mm),
  block_size(block_size)
{}

vector<int> TwoLevelUnifiedCache::load_block(int base) {
    vector<int> blk(block_size);
    for (int i = 0; i < block_size; i++)
        mem.read_physical(base + i, blk[i]);
    return blk;
}

int TwoLevelUnifiedCache::read(int address) {

    int val;

    if (L1.read(address, val))
        return val;

    if (L2.read(address, val)) {

        int base = (address / block_size) * block_size;
        auto blk = L2.get_block(address);

        L1.insert_block(base, blk);
        return val;
    }

    int base = (address / block_size) * block_size;
    auto blk = load_block(base);

    L2.insert_block(base, blk);
    L1.insert_block(base, blk);

    int off = address % block_size;
    return blk[off];
}

void TwoLevelUnifiedCache::write(int address, int value) {

    if (L1.write(address, value))
        return;

    if (L2.write(address, value)) {

        int base = (address / block_size) * block_size;
        auto blk = L2.get_block(address);

        L1.insert_block(base, blk);
        L1.write(address, value);
        return;
    }

    int base = (address / block_size) * block_size;
    auto blk = load_block(base);

    L2.insert_block(base, blk);
    L1.insert_block(base, blk);

    L1.write(address, value);
}

void TwoLevelUnifiedCache::invalidate_range(int start, int end) {
    L1.invalidate_range(start, end);
    L2.invalidate_range(start, end);
}

void TwoLevelUnifiedCache::stats() {
    cout << "\n===== L1 =====\n";
    cout << "Hits   : " << L1.hits << "\n";
    cout << "Misses : " << L1.misses << "\n";

    cout << "\n===== L2 =====\n";
    cout << "Hits   : " << L2.hits << "\n";
    cout << "Misses : " << L2.misses << "\n";
}
