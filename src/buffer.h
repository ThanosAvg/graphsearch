#ifndef BUFFER_H
#define BUFFER_H

#include "list_node.h"
#include "types.h"
#include <stdint.h>

const uint32_t INITIAL_BUFFER_SIZE = 1048576;

class Buffer{
public:
    Buffer();
    ~Buffer();

    // Allocates a new listnode in buffer and returns its pointer (offset)
    // or PTR_NULL if allocation failed
    ptr allocNewNode();

    // Finds the list node at the given offset and returns a pointer to it
    ListNode* getListNode(ptr nodeIndex);
private:
    ListNode* bufferSpace_;
    ptr numberOfNodes_;
    ptr bufferSize_;
    ListNode templateListNode_;
};

#endif
