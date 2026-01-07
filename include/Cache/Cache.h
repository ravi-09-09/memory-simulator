#pragma once
#include <bits/stdc++.h>
using namespace std;

// Forward declaration instead of including allocator.cpp
class MemoryManager;

/* ===================== BASIC TYPES ===================== */

enum class CacheType {
    DIRECT,
    SET_ASSOC
};

enum class Replacement {
    FIFO,
    LRU
};

struct Line {
    int tag   = -1;
    bool valid = false;
    bool dirty = false;
    vector<int> block;
    int timestamp = 0;

    Line() = default;
};


/* ===================== CACHE LEVEL ===================== */

class CacheLevel {

    CacheType type;
    Replacement policy;

    int cache_size;
    int block_size;
    int associativity;
    int num_sets;

    vector<vector<Line>> sets;
    vector<queue<int>> fifo;
    int time_counter = 0;

public:
    int hits = 0, misses = 0;

    CacheLevel(CacheType type,
               Replacement policy,
               int cache_size,
               int block_size,
               int associativity = 1);

    bool read(int address, int &value);
    void insert_block(int address, const vector<int> &blk);
    bool write(int address, int value);
    vector<int> get_block(int address);

    void invalidate_range(int start, int end);

private:
    int get_index(int address);
    int get_tag(int address);
    int get_offset(int address);
    int choose_victim(int set_index);
};


/* ===================== TWO LEVEL UNIFIED CACHE ===================== */

class TwoLevelUnifiedCache {

    CacheLevel L1;
    CacheLevel L2;
    MemoryManager &mem;
    int block_size;

public:

    TwoLevelUnifiedCache(
        CacheType type1, Replacement p1,
        int l1_lines, int assoc1,

        CacheType type2, Replacement p2,
        int l2_lines, int assoc2,

        int block_size,
        MemoryManager &mm
    );

    vector<int> load_block(int base);

    int read(int address);
    void write(int address, int value);

    void invalidate_range(int start, int end);

    void stats();
};
