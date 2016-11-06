#include <stdlib.h>

#include "node_index.h"
#include "buffer.h"
#include <iostream>

NodeIndex::NodeIndex(Buffer* buffer){
    this->buffer_ = buffer;
    this->hash_ = new Hash<NodeIndexData>(this->hashBuckets_);
}

NodeIndex::~NodeIndex(){
    delete this->hash_;
}

uint32_t NodeIndex::getCurrentSize(){
    return this->hash_->getSize();
}

uint32_t NodeIndex::getMaxSize(){
    return getCurrentSize(); // TODO: deprecate this
}

bool NodeIndex::insertNode(uint32_t nodeId){
    if(this->buffer_ == 0){
        return false;
    }

    // Create new node in buffer
    ptr nodeLoc;
    uint32_t neighborCount = 0;
    nodeLoc = this->buffer_->allocNewNode();

    NodeIndexData nodeData;
    nodeData.nodeLoc_ = nodeLoc;
    nodeData.neighborCount_ = 0;
    nodeData.lastFree_ = nodeLoc;

    this->hash_->add(nodeData, nodeId); // Data , Key pair
    return true;
}

ptr NodeIndex::getListHead(uint32_t nodeId){
    ResultCode rescode;
    NodeIndexData element = this->hash_->get(nodeId, rescode);

    if(rescode == NOT_FOUND){
        return PTR_NULL;
    }

    return element.nodeLoc_;
}

ptr NodeIndex::getListTail(uint32_t nodeId){
    ResultCode rescode;
    NodeIndexData element = this->hash_->get(nodeId, rescode);

    if(rescode == NOT_FOUND){
        return PTR_NULL;
    }

    return element.lastFree_;
}

uint32_t NodeIndex::getNeighborCount(uint32_t nodeId){
    ResultCode rescode;
    NodeIndexData element = this->hash_->get(nodeId, rescode);

    if(rescode == NOT_FOUND){
        return 0;
    }

    return element.neighborCount_;
}

void NodeIndex::incrementNeighbors(uint32_t nodeId){
    ResultCode rescode;
    NodeIndexData element = this->hash_->get(nodeId, rescode);

    if(rescode == NOT_FOUND){
        return;
    }

    element.neighborCount_ ++;
    this->hash_->update(element, nodeId);
}

void NodeIndex::setListTail(uint32_t nodeId, ptr tail){
    ResultCode rescode;
    NodeIndexData element = this->hash_->get(nodeId, rescode);

    if(rescode == NOT_FOUND){
        return;
    }

    element.lastFree_ = tail;
    this->hash_->update(element, nodeId);
}
