#include "VirtualMemory/VirtualMemory.h"
#include "Cache/Cache.h"
#include "Allocator/MemoryManager.h"

#include <bits/stdc++.h>
using namespace std;

/* ===================== CONSTRUCTOR ===================== */

VirtualMemory::VirtualMemory(int virtual_size,
                             int ram_size,
                             int page_size,
                             TwoLevelUnifiedCache &cache,
                             MemoryManager &mm,
                             PagePolicy policy)
    : virtual_size(virtual_size),
      ram_size(ram_size),
      page_size(page_size),
      cache(cache),
      mm(mm),
      policy(policy)
{
    num_pages  = (virtual_size + page_size - 1) / page_size;
    num_frames = ram_size / page_size;

    disk.assign(virtual_size, 0);

    frames.assign(num_frames, Frame());
    page_table.assign(num_pages, PageEntry());

    last_use.assign(num_pages, -1);
}

/* ===================== HELPERS ===================== */

void VirtualMemory::invalidate_frame_in_cache(int frame_base_phys_addr)
{
    int start = frame_base_phys_addr;
    int end   = frame_base_phys_addr + page_size - 1;

    cache.invalidate_range(start, end);
}

int VirtualMemory::choose_victim_frame()
{
    if (policy == PagePolicy::FIFO) {
        int frame = fifo.front();
        fifo.pop();
        return frame;
    }

    int victim_page = -1;
    int best_time = INT_MAX;

    for (int page = 0; page < num_pages; page++) {
        if (page_table[page].valid &&
            last_use[page] < best_time)
        {
            best_time = last_use[page];
            victim_page = page;
        }
    }

    return page_table[victim_page].frame;
}

/* ===================== PAGE FAULT ===================== */

void VirtualMemory::handle_page_fault(int page)
{
    page_faults++;

    int frame = -1;

    // find free frame
    for (int i = 0; i < num_frames; i++) {
        if (frames[i].free) {
            frame = i;
            break;
        }
    }

    // eviction if no free frame
    if (frame == -1) {

        frame = choose_victim_frame();
        evictions++;

        int victim_page = frames[frame].owner_page;
        PageEntry &victim = page_table[victim_page];

        int base_ram  = frame * page_size;
        int base_disk = victim_page * page_size;

        // write back if dirty
        if (victim.dirty) {
            for (int i = 0; i < page_size; i++) {
                int val;
                mm.read_physical(base_ram + i, val);
                disk[base_disk + i] = val;
            }
        }

        invalidate_frame_in_cache(base_ram);

        victim.valid = false;
        victim.frame = -1;
        victim.dirty = false;
    }

    int base_ram  = frame * page_size;
    int base_disk = page * page_size;

    // load from disk to RAM
    for (int i = 0; i < page_size && base_disk + i < virtual_size; i++) {
        int val = disk[base_disk + i];
        mm.write_physical(base_ram + i, val);
    }

    // update mappings
    page_table[page].frame = frame;
    page_table[page].valid = true;

    frames[frame].free = false;
    frames[frame].owner_page = page;

    fifo.push(frame);

    current_time++;
    last_use[page] = current_time;
}

/* ===================== TRANSLATE ===================== */

int VirtualMemory::translate(int vaddr)
{
    if (vaddr < 0 || vaddr >= virtual_size)
        return -1;

    int page   = vaddr / page_size;
    int offset = vaddr % page_size;

    PageEntry &p = page_table[page];

    if (!p.valid)
        handle_page_fault(page);
    else
        page_hits++;

    current_time++;
    last_use[page] = current_time;

    return page_table[page].frame * page_size + offset;
}

/* ===================== PUBLIC API ===================== */

int VirtualMemory::read(int vaddr)
{
    int phys = translate(vaddr);
    if (phys == -1) return -1;

    return cache.read(phys);
}

void VirtualMemory::write(int vaddr, int value)
{
    int phys = translate(vaddr);
    if (phys == -1) return;

    cache.write(phys, value);
    mm.write_physical(phys, value);

    // update disk copy
    disk[vaddr] = value;

    int page = vaddr / page_size;
    page_table[page].dirty = true;
}

void VirtualMemory::stats()
{
    cout << "\n===== VIRTUAL MEMORY STATS =====\n";
    cout << "Pages        : " << num_pages << "\n";
    cout << "Frames       : " << num_frames << "\n";
    cout << "Page Faults  : " << page_faults << "\n";
    cout << "Page Hits    : " << page_hits << "\n";
    cout << "Evictions    : " << evictions << "\n";
    cout << "================================\n\n";
}
