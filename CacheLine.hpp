// CacheLine.hpp

#ifndef CACHELINE_HPP
#define CACHELINE_HPP

#include <cstdint>

enum CoherencyState {
    MODIFIED,
    OWNED,
    EXCLUSIVE,
    SHARED,
    INVALID,
    FORWARD
};

class CacheLine {
private:
    CoherencyState state;
    uint32_t lruState;
    uint32_t tag;
    bool dirty;

public:
    CacheLine() : state(INVALID), lruState(0), tag(0), dirty(false) {}

    void setState(CoherencyState newState) { state = newState; }
    void setLRU(uint32_t lru) { lruState = lru; }
    void setTag(uint32_t newTag) { tag = newTag; }
    void setDirty(bool isDirty) { dirty = isDirty; }

    CoherencyState getState() const { return state; }
    uint32_t getLRU() const { return lruState; }
    uint32_t getTag() const { return tag; }
    bool isDirty() const { return dirty; }
};

#endif