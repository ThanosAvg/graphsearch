#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include "list_node.h"
#include "types.h"

class Buffer{
public:
	Buffer();
	ptr allocNewNode();
	ListNode* getListNode(ptr);
	~Buffer();
private:
	ListNode** bufferSpace;
	ptr nextEmptySlot;
};

#endif
