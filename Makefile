CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra -O2

SRC_ALLOC = src/Allocator/MemoryManager.cpp
SRC_CACHE = src/Cache/Cache.cpp
SRC_VM    = src/VirtualMemory/VirtualMemory.cpp

OBJS = $(SRC_ALLOC:.cpp=.o) \
       $(SRC_CACHE:.cpp=.o) \
       $(SRC_VM:.cpp=.o)

# --------------------------------------------------
# Build everything by default
# --------------------------------------------------
all: simulator vm_normal cache_hard vm_hard

# --------------------------------------------------
# Main simulator (uses src/main.cpp)
# --------------------------------------------------
simulator: src/main.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# --------------------------------------------------
# Stress / test programs
# --------------------------------------------------
vm_normal: tests/vm_normal.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

cache_hard: tests/cache_hard_stress.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

vm_hard: tests/vm_hard_stress.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# --------------------------------------------------
# Clean build files
# --------------------------------------------------
clean:
	rm -f $(OBJS) simulator vm_normal cache_hard vm_hard
