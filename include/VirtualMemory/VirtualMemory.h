#pragma once
#include <bits/stdc++.h>
using namespace std;

// Forward declarations
class TwoLevelUnifiedCache;
class MemoryManager;

enum class PagePolicy {
    FIFO,
    LRU
};

struct Frame {
    bool free = true;
    int owner_page = -1;
};

struct PageEntry {
    int frame = -1;
    bool valid = false;
    bool dirty = false;
};

class VirtualMemory {

    int virtual_size;
    int ram_size;
    int page_size;

    int num_pages;
    int num_frames;

    vector<int> disk;
    vector<Frame> frames;
    vector<PageEntry> page_table;

    queue<int> fifo;

    vector<int> last_use;
    int current_time = 0;

    PagePolicy policy;
    TwoLevelUnifiedCache &cache;
    MemoryManager &mm;

    int page_faults = 0;
    int page_hits   = 0;
    int evictions   = 0;

public:

    VirtualMemory(int virtual_size,
                  int ram_size,
                  int page_size,
                  TwoLevelUnifiedCache &cache,
                  MemoryManager &mm,
                  PagePolicy policy);

    int read(int vaddr);
    void write(int vaddr, int value);
    void stats();

private:

    void invalidate_frame_in_cache(int frame_base_phys_addr);

    int choose_victim_frame();
    void handle_page_fault(int page);
    int translate(int vaddr);
};
