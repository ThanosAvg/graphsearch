#ifndef NODE_INDEX_H
#define NODE_INDEX_H

#include <stdint.h>

#include "types.h"
#include "buffer.h"

const uint32_t INITIAL_SIZE = 1024;
typedef ptr NodeIndexData;

class NodeIndex{
public:
    NodeIndex(Buffer* buffer);
    ~NodeIndex();
    bool insertNode(uint32_t nodeId);
    ptr getListHead(uint32_t nodeId);
    uint32_t getCurrentSize();
    uint32_t getMaxSize();
private:
    NodeIndexData* data_;
    Buffer* buffer_;
    uint32_t currentSize_;
    uint32_t maxSize_;
};

#endif
