#include <stdint.h>

#include "types.h"

const int N = 10; // number of neighbors per list node

class ListNode{
public:
    ListNode();
    void addNeighbor(uint32_t id);
    void addEdgeProperty(uint32_t);
private:
    uint32_t neighbor[N];     // the ids of the neighbor nodes
    uint32_t edgeProperty[N]; // property for each edge
    ptr nextListNode;         // offset of next list node
};
