#include <stdint.h>

#include "types.h"

const long INITIAL_SIZE = 20;

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
private:
    NodeIndexData data[INITIAL_SIZE];
    Buffer* buffer;
};
