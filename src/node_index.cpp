#include <iostream>
#include <stdlib.h>
#include "node_index.h"
#include "buffer.h"

NodeIndex::NodeIndex(Buffer* buffer){
    this->buffer_ = buffer;
    this->currentSize_ = this->startSize_;
    std::cout << "Started allocating array" << std::endl;
    //this->array_ = (NodeIndexData*) calloc(this->currentSize_, sizeof(NodeIndexData));
    this->array_ = (NodeIndexData*) malloc(this->currentSize_ * sizeof(NodeIndexData));
    memset(this->array_, 0, this->currentSize_ * sizeof(NodeIndexData));
    std::cout << "Array allocated" << std::endl;
}

NodeIndex::~NodeIndex(){
    free(this->array_);
}

uint32_t NodeIndex::getCurrentSize(){
    return this->currentSize_;
}

uint32_t NodeIndex::getMaxSize(){
    return getCurrentSize(); // TODO: deprecate this
}

bool NodeIndex::insertNode(uint32_t nodeId){
    //if(this->buffer_ == 0){
    //    return false;
    //}

    // Create new node in buffer
    ptr nodeLoc;
    nodeLoc = this->buffer_->allocNewNode();

    NodeIndexData nodeData;
    nodeData.nodeLoc_ = nodeLoc;
    nodeData.neighborCount_ = 0;
    nodeData.lastFree_ = nodeLoc;
    nodeData.flag = 1;

    //this->hash_->add(nodeData, nodeId); // Data , NodeId pair
    if(nodeId >= this->currentSize_){
        std::cout << "Scaling started..." << std::endl;
        // Scale array
        NodeIndexData *resized;
        int ntimes = 2;
        while(ntimes * this->currentSize_ < nodeId + 1){
            ntimes *= 2;
        }
        resized = (NodeIndexData*) realloc(this->array_, this->currentSize_ * sizeof(NodeIndexData) * ntimes + 1);
        if(resized == NULL){
            // Realloc failed, free resources and fail
            free(this->array_);
            return false;
        }
        this->array_ = resized;
        std::cout << "Scaling done" << std::endl;
        this->currentSize_ = this->currentSize_ * ntimes + 1;
    }

    // Add element to array
    this->array_[nodeId] = nodeData; 
    return true;
}

ptr NodeIndex::getListHead(uint32_t nodeId){
    if(nodeId >= this->currentSize_){
        return PTR_NULL;
    }
    else if(this->array_[nodeId].flag == 0){
        return PTR_NULL;
    }
    else{
        return array_[nodeId].nodeLoc_;
    }
}

ptr NodeIndex::getListTail(uint32_t nodeId){
    if(nodeId >= this->currentSize_){
        return PTR_NULL;
    }
    else if(this->array_[nodeId].flag == 0){
        return PTR_NULL;
    }
    else{
        return array_[nodeId].lastFree_;
    }
}

long NodeIndex::getNeighborCount(uint32_t nodeId){
    if(nodeId >= this->currentSize_){
        return -1;
    }
    else if(this->array_[nodeId].flag == 0){
        return -1;
    }
    else{
        return array_[nodeId].neighborCount_;
    }
}

void NodeIndex::incrementNeighbors(uint32_t nodeId){
    if(nodeId >= this->currentSize_){
        return ;
    }
    else if(this->array_[nodeId].flag == 0){
        return ;
    }
    else{
        array_[nodeId].neighborCount_++;
    }
}

void NodeIndex::setListTail(uint32_t nodeId, ptr tail){
    if(nodeId >= this->currentSize_){
        return ;
    }
    else if(this->array_[nodeId].flag == 0){
        return ;
    }
    else{
        array_[nodeId].lastFree_ = tail;
    }
}
