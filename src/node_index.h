#ifndef NODE_INDEX_H
#define NODE_INDEX_H

#include <stdint.h>

#include "types.h"
#include "buffer.h"
#include "table.h"
#include "hashtable.h"

//typedef ptr NodeIndexData;
struct NodeIndexData{
    ptr nodeLoc_;
    //uint32_t neighborCount_;
    long neighborCount_;
    ptr lastFree_;
    int flag;
};

class NodeIndex{
public:
    NodeIndex(Buffer* buffer);
    ~NodeIndex();
    bool insertNode(uint32_t nodeId);
    ptr getListHead(uint32_t nodeId);
    ptr getListTail(uint32_t nodeId);
    void setListTail(uint32_t nodeId, ptr tail);
    long getNeighborCount(uint32_t nodeId);
    uint32_t getCurrentSize();
    uint32_t getMaxSize();
    void incrementNeighbors(uint32_t nodeId);
private:
    static const long startSize_ = 7000000;
    Buffer* buffer_;
    NodeIndexData* array_;
    uint32_t currentSize_;
};

#endif
