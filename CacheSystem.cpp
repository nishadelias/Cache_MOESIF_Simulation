// CacheSystem.cpp

#include "CacheSystem.hpp"
#include <iostream>

void CacheSystem::handleRead(int coreId, uint32_t tag) {
    // Step 1: Check if the line exists in the requesting core's cache
    int lineIndex = caches[coreId].findLine(tag);
    
    if (lineIndex != -1) {  // Cache hit
        CacheLine& line = caches[coreId].getLine(lineIndex);
        CoherencyState currentState = line.getState();
        
        // Update statistics and LRU
        cacheHits++;
        caches[coreId].updateLRU(lineIndex);
        
        // Broadcast if not in Exclusive state
        if (currentState != EXCLUSIVE) {
            broadcasts++;
        }
        
        return;
    }
    
    // Cache miss - need to handle coherency
    cacheMisses++;
    broadcasts++;
    
    // Step 2: Check other caches for the line
    bool foundInOtherCache = false;
    int providingCore = -1;
    CoherencyState providingState;
    
    for (int i = 0; i < NUM_CORES; i++) {
        if (i == coreId) continue;
        
        int otherLineIndex = caches[i].findLine(tag);
        if (otherLineIndex != -1) {
            CacheLine& otherLine = caches[i].getLine(otherLineIndex);
            foundInOtherCache = true;
            providingCore = i;
            providingState = otherLine.getState();
            
            // Update state of the providing cache line
            switch (otherLine.getState()) {
                case MODIFIED:
                    otherLine.setState(OWNED);
                    otherLine.setDirty(true);
                    break;
                case EXCLUSIVE:
                    otherLine.setState(SHARED);
                    break;
                case OWNED:
                    // Stays in Owned state
                    break;
                case SHARED:
                    // Stays in Shared state
                    break;
                default:
                    break;
            }
        }
    }
    
    // Step 3: Install the line in the requesting cache
    int newLineIndex = caches[coreId].findFirstInvalid();
    if (newLineIndex == -1) {
        // Need to evict - find LRU line
        newLineIndex = caches[coreId].findLRULine();
        CacheLine& lruLine = caches[coreId].getLine(newLineIndex);
        
        // Check if we need to writeback the evicted line
        if (lruLine.isDirty() && (lruLine.getState() == MODIFIED || lruLine.getState() == OWNED)) {
            writebacks++;
        }
    }
    
    // Install the new line
    CacheLine& newLine = caches[coreId].getLine(newLineIndex);
    newLine.setTag(tag);
    newLine.setDirty(false);
    
    if (foundInOtherCache) {
        cacheToCache++;
        newLine.setState(SHARED);
    } else {
        newLine.setState(EXCLUSIVE);
    }
    
    caches[coreId].updateLRU(newLineIndex);
}


void CacheSystem::handleWrite(int coreId, uint32_t tag) {
    // Step 1: Check if the line exists in the requesting core's cache
    int lineIndex = caches[coreId].findLine(tag);

    if (lineIndex != -1) {  // Cache hit
        CacheLine &line = caches[coreId].getLine(lineIndex);
        CoherencyState currentState = line.getState();
        
        cacheHits++;
        
        // If not in E state, need to broadcast and invalidate other copies
        if (currentState != EXCLUSIVE) {
            broadcasts++;
            // Invalidate other copies
            for (int i = 0; i < NUM_CORES; i++) {
                if (i == coreId) continue;
                
                int otherLineIndex = caches[i].findLine(tag);
                if (otherLineIndex != -1) {
                    CacheLine &otherLine = caches[i].getLine(otherLineIndex);
                    if (otherLine.isDirty() && 
                        (otherLine.getState() == MODIFIED || otherLine.getState() == OWNED)) {
                        writebacks++;
                    }
                    otherLine.setState(INVALID);
                }
            }
        }
        
        // Update state of writing cache
        line.setState(MODIFIED);
        line.setDirty(true);
        caches[coreId].updateLRU(lineIndex);
        return;
    }

    // Cache miss - need to handle coherency
    cacheMisses++;
    broadcasts++;

    // Step 2: Invalidate other copies and obtain line from memory or other caches
    bool foundInOtherCache = false;
    for (int i = 0; i < NUM_CORES; i++) {
        if (i == coreId) continue;

        int otherLineIndex = caches[i].findLine(tag);
        if (otherLineIndex != -1) {
            CacheLine &otherLine = caches[i].getLine(otherLineIndex);
            switch (otherLine.getState()) {
                case MODIFIED:
                case OWNED:
                case FORWARD:
                    if (otherLine.isDirty()) {
                        // Data needs to be written back to memory
                        writebacks++;
                        otherLine.setDirty(false);
                    }
                    otherLine.setState(INVALID);
                    break;
                case SHARED:
                case EXCLUSIVE:
                    otherLine.setState(INVALID);
                    // No writeback needed for S or E states
                    break;
                default:
                    break;
            }
            foundInOtherCache = true;
        }
    }

    // Step 3: Install the line in the requesting cache
    int newLineIndex = caches[coreId].findFirstInvalid();
    if (newLineIndex == -1) {
        // Need to evict - find LRU line
        newLineIndex = caches[coreId].findLRULine();
        CacheLine &lruLine = caches[coreId].getLine(newLineIndex);

        // Check if we need to write back the evicted line
        if (lruLine.isDirty() && (lruLine.getState() == MODIFIED || lruLine.getState() == OWNED)) {
            writebacks++;
        }
    }

    // Install the new line and set state to 'Modified'
    CacheLine &newLine = caches[coreId].getLine(newLineIndex);
    newLine.setTag(tag);
    newLine.setState(MODIFIED);
    newLine.setDirty(true);

    caches[coreId].updateLRU(newLineIndex);
}
