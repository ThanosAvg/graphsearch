#include "graph.h"

Graph::Graph(){
    this->incomingBuffer_ = new Buffer();
    this->outgoingBuffer_ = new Buffer();
    this->incomingIndex_ = new NodeIndex(this->incomingBuffer_);
    this->outgoingIndex_ = new NodeIndex(this->outgoingBuffer_);
}

bool Graph::add(uint32_t from, uint32_t to){
    bool suc1, suc2;
    suc1 = this->addToPair(this->incomingIndex_, this->incomingBuffer_, to);
    suc2 = this->addToPair(this->outgoingIndex_, this->outgoingBuffer_, from);
    return suc1 && suc2;
}

bool Graph::addToPair(NodeIndex* index, Buffer* buffer, uint32_t id){
    // Check if node exists
    ptr lNodePtr = index->getListHead(id);
    if(lNodePtr == PTR_NULL){
        // Create node
        index->insertNode(id);
        lNodePtr = index->getListHead(id);
    }
    // Get actual list node
    ListNode* listNode = buffer->getListNode(lNodePtr);

    // Skip if full
    while(listNode->getNeighborMax() == listNode->getNeighborCount()){
        listNode = buffer->getListNode(listNode->getNextListNode());
    }
    listNode->addNeighbor(id);
    return true;
}
