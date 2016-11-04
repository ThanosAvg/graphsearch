#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include "list_node.h"
#include "types.h"

const uint32_t INITIAL_BUFFER_SIZE = 1024;

class Buffer{
public:
	Buffer();
	ptr allocNewNode();
	ListNode* getListNode(ptr);
	~Buffer();
private:
	ListNode* bufferSpace_;
	ptr numberOfNodes_;
	ptr bufferSize_;
};

#endif
