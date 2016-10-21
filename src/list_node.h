#ifndef LIST_NODE_H
#define LIST_NODE_H

#include <stdint.h>

#include "types.h"

class ListNode{
public:
    ListNode();
    void addNeighbor(uint32_t id);
    void addEdgeProperty(uint32_t);
private:
    static const int neighborMax = 10; // number of neighbors per list node
    uint32_t neighbor[neighborMax];       // the ids of the neighbor nodes
    uint32_t edgeProperty[neighborMax];   // property for each edge
    ptr nextListNode;           // offset of next list node
    uint32_t neighborCount;     // number of edges in list node
};

#endif
