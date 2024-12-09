// Cache.hpp

#ifndef CACHE_HPP
#define CACHE_HPP

#include "CacheLine.hpp"
#include <vector>

class Cache {
private:
    std::vector<CacheLine> lines;
    static const int NUM_SETS = 4;

public:
    Cache() : lines(NUM_SETS) {
        // Initialize cache lines with default values
        for (int i = 0; i < NUM_SETS; i++) {
            // CHECK to make sure this is correct initialization
            lines[i].setLRU(i);
        }
    }

    CacheLine& getLine(int index) { return lines[index]; }
    
    // Find a line by tag, returns -1 if not found
    int findLine(uint32_t tag) {
        for (int i = 0; i < NUM_SETS; i++) {
            if (lines[i].getState() != INVALID && 
                lines[i].getTag() == tag) {
                return i;
            }
        }
        return -1;
    }

    // Find first invalid line, returns -1 if none found
    int findFirstInvalid() {
        for (int i = 0; i < NUM_SETS; i++) {
            if (lines[i].getState() == INVALID) {
                return i;
            }
        }
        return -1;
    }

    // Find LRU line
    int findLRULine() {
        for (int i = 0; i < NUM_SETS; i++) {
            if (lines[i].getLRU() == 0) {
                return i;
            }
        }
        return 0; // Should never happen if LRU is maintained correctly
    }

    // Update LRU states after access
    void updateLRU(int accessedIndex) {
        uint32_t oldLRU = lines[accessedIndex].getLRU();
        for (int i = 0; i < NUM_SETS; i++) {
            uint32_t currentLRU = lines[i].getLRU();
            if (currentLRU > oldLRU) {
                lines[i].setLRU(currentLRU - 1);
            }
        }
        lines[accessedIndex].setLRU(NUM_SETS - 1);
    }
};

#endif