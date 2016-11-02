#include "graph.h"
#include "queue.h"

Graph::Graph(){
    this->incomingBuffer_ = new Buffer();
    this->outgoingBuffer_ = new Buffer();
    this->incomingIndex_ = new NodeIndex(this->incomingBuffer_);
    this->outgoingIndex_ = new NodeIndex(this->outgoingBuffer_);
}

bool Graph::add(uint32_t from, uint32_t to){
    bool suc1, suc2;
    suc1 = this->addToPair(this->incomingIndex_, this->incomingBuffer_, to, from);
    suc2 = this->addToPair(this->outgoingIndex_, this->outgoingBuffer_, from, to);
    return suc1 && suc2;
}

long Graph::query(uint32_t from, uint32_t to){
    Queue startQueue;
    Queue endQueue;

    startQueue.enqueue(from);
    endQueue.enqueue(to);


}

bool Graph::addToPair(NodeIndex* index, Buffer* buffer, uint32_t target, uint32_t node){
    // Check if node exists
    ptr lNodePtr = index->getListHead(target);
    if(lNodePtr == PTR_NULL){
        // Create node
        index->insertNode(target);
        lNodePtr = index->getListHead(target);
    }

    // Get actual list node
    ListNode* listNode = buffer->getListNode(lNodePtr);

    // Check if neighbor already exists
    if(listNode->containsNeighbor(node)){
        return true; // Its already there
    }

    // Skip if full
    while(listNode->getNeighborMax() == listNode->getNeighborCount()){
        // Check if next list node exists
        if(listNode->getNextListNode() == PTR_NULL){
            // We create the next list node
            ptr newAddr;
            newAddr = buffer->allocNewNode();

            if(newAddr == PTR_NULL){
                return false; // We failed to allocate
            }

            listNode->setNextListNode(newAddr);
        }
        listNode = buffer->getListNode(listNode->getNextListNode());

        // Check if neighbor already exists
        if(listNode->containsNeighbor(node)){
            return true; // Its already there
        }
    }
    listNode->addNeighbor(node);
    return true;
}
