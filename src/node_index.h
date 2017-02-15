#ifndef NODE_INDEX_H
#define NODE_INDEX_H

#include "types.h"
#include "buffer.h"
#include "hashtable.h"

#include <stdint.h>

struct NodeIndexData{
    ptr nodeLoc_;
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
    void resetCursor();
    uint32_t getNextId(bool &error);
    int32_t getMaxId();
private:
    static const long startSize_ = 7000000;
    Buffer* buffer_;       // Pointer to buffer associated with this index
    NodeIndexData* array_; // Array holding the node data
    uint32_t currentSize_; // Current array size (doubles if needed)
    int32_t cursor_;       // Last getNextId position
    int32_t maxId_;        // Maximum node id
};

#endif
