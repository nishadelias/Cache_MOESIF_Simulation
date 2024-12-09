// test_cache.cpp
#include "CacheSystem.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

class TestCache {
public:
    static void runAllTests() {
        testSimpleRead();
        testReadHit();
        testReadMiss();
        testReadShared();
        testReadModified();
        testEviction();
        std::cout << "All tests passed!" << std::endl;
    }

private:
    static void testSimpleRead() {
        CacheSystem system;
        system.handleRead(0, 100);  // First read should be a miss, get Exclusive
        assert(system.getCacheMisses() == 1);
        assert(system.getCacheHits() == 0);
        assert(system.getBroadcasts() == 1);
        assert(system.getCacheToCache() == 0);
        std::cout << "Simple read test passed" << std::endl;
    }
    
    static void testReadHit() {
        CacheSystem system;
        system.handleRead(0, 100);  // Miss, get Exclusive
        system.handleRead(0, 100);  // Hit in Exclusive
        assert(system.getCacheMisses() == 1);
        assert(system.getCacheHits() == 1);
        assert(system.getBroadcasts() == 1);  // No broadcast for hit in E state
        std::cout << "Read hit test passed" << std::endl;
    }
    
    static void testReadMiss() {
        CacheSystem system;
        system.handleRead(0, 100);  // Core 0 gets Exclusive
        system.handleRead(1, 100);  // Core 1 reads - should cause transition to Shared
        assert(system.getCacheMisses() == 2);
        assert(system.getCacheHits() == 0);
        assert(system.getBroadcasts() == 2);
        assert(system.getCacheToCache() == 1);
        std::cout << "Read miss test passed" << std::endl;
    }
    
    static void testReadShared() {
        CacheSystem system;
        system.handleRead(0, 100);  // Core 0 gets Exclusive
        system.handleRead(1, 100);  // Core 1 reads - goes to Shared
        system.handleRead(2, 100);  // Core 2 reads - should get from Shared
        assert(system.getCacheToCache() == 2);
        assert(system.getBroadcasts() == 3);
        std::cout << "Read shared test passed" << std::endl;
    }
    
    static void testReadModified() {
        CacheSystem system;
        // Setup: Get line in M state
        system.handleRead(0, 100);   // Get Exclusive
        system.handleWrite(0, 100);  // Upgrade to Modified
        system.handleRead(1, 100);   // Should cause writeback
        assert(system.getWritebacks() == 1);
        assert(system.getCacheToCache() == 1);
        std::cout << "Read modified test passed" << std::endl;
    }
    
    static void testEviction() {
        CacheSystem system;
        // Fill cache with 4 lines
        for (int i = 0; i < 4; i++) {
            system.handleRead(0, i * 100);
        }
        // This should cause eviction
        system.handleRead(0, 500);
        assert(system.getCacheMisses() == 5);
        std::cout << "Eviction test passed" << std::endl;
    }
};