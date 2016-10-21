#ifndef NODE_INDEX_H
#define NODE_INDEX_H

#include <stdint.h>

#include "types.h"
#include "buffer.h"

const uint32_t INITIAL_SIZE = 20;

/* Helper struct to store nodeid, pointer tuples. */
class NodeIndexData{
public:
    uint32_t nodeid;
    ptr pointer;
};

class NodeIndex{
public:
    NodeIndex(Buffer* buffer);
    ~NodeIndex();
    bool insertNode(uint32_t nodeId);
    ptr getListHead(uint32_t nodeId);
    uint32_t getCurrentSize();
    uint32_t getMaxSize();
private:
    NodeIndexData* data;
    Buffer* buffer;
    uint32_t currentSize;
    uint32_t maxSize;
};

#endif
