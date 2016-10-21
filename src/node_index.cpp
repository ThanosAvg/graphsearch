#include <stdlib.h>

#include "node_index.h"
#include "buffer.h"

NodeIndex::NodeIndex(Buffer* buffer){
    this->buffer = buffer;
    this->data = (NodeIndexData*) malloc(sizeof(NodeIndexData) * INITIAL_SIZE);
    this->currentSize = 0;
    this->maxSize = INITIAL_SIZE;
}

NodeIndex::~NodeIndex(){
    free(data);
}

uint32_t NodeIndex::getCurrentSize(){
    return this->currentSize;
}

uint32_t NodeIndex::getMaxSize(){
    return this->maxSize;
}

bool NodeIndex::insertNode(uint32_t nodeId){
    if(this->buffer == 0){
	return false;
    }

    // Create new node in buffer
    ptr nodeLoc;
    nodeLoc = this->buffer->allocNewNode();

    // Populate NodeIndexData
    NodeIndexData nodePair;
    nodePair.nodeid = nodeId;
    nodePair.pointer = nodeLoc;

    // See if we have space in array
    if(this->currentSize == this->maxSize){
	// We are full, double max size
	NodeIndexData* resized;
	resized = (NodeIndexData*) realloc(this->data, this->maxSize * 2 * sizeof(NodeIndexData));
	if(resized == NULL){
	    // Realloc failed, free resources and fail
	    free(this->data);
	    return false;
	}

	// Reallocation was successful, overwrite data
	this->data = resized;
	this->maxSize = this->maxSize * 2;
    }

    this->data[this->currentSize] = nodePair;
    this->currentSize++;
    return true;
}

ptr NodeIndex::getListHead(uint32_t nodeId){
    
}
