// CacheSystem.hpp

#ifndef CACHESYSTEM_HPP
#define CACHESYSTEM_HPP

#include "Cache.hpp"
#include <array>

class CacheSystem {
private:
    static const int NUM_CORES = 4;
    std::array<Cache, NUM_CORES> caches;
    
    // Statistics
    int cacheHits;
    int cacheMisses;
    int writebacks;
    int broadcasts;
    int cacheToCache;

public:
    CacheSystem() : 
        cacheHits(0), 
        cacheMisses(0), 
        writebacks(0), 
        broadcasts(0), 
        cacheToCache(0) {}

    void handleRead(int coreId, uint32_t tag);
    void handleWrite(int coreId, uint32_t tag);
    
    // Statistics getters
    int getCacheHits() const { return cacheHits; }
    int getCacheMisses() const { return cacheMisses; }
    int getWritebacks() const { return writebacks; }
    int getBroadcasts() const { return broadcasts; }
    int getCacheToCache() const { return cacheToCache; }
};

#endif