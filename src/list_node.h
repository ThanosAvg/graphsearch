#ifndef LIST_NODE_H
#define LIST_NODE_H

#include <stdint.h>

#include "types.h"

class ListNode{
public:
    ListNode();
    bool addNeighbor(uint32_t id);
    bool addEdgeProperty(uint32_t);
    uint32_t* getNeighborsPtr();
    uint32_t getNeighborCount();
    uint32_t getNeighborMax();
    ptr getNextListNode();
    void setNextListNode(ptr next);
private:
    static const uint32_t neighborMax_ = 10; // number of neighbors per list node
    uint32_t neighbors_[neighborMax_];       // the ids of the neighbor nodes
    uint32_t edgeProperty_[neighborMax_];   // property for each edge
    ptr nextListNode_;           // offset of next list node
    uint32_t neighborCount_;     // number of edges in list node
};

#endif
