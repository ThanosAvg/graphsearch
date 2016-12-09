#include "graph.h"
#include "queue.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <sys/time.h>

using namespace std;

Graph::Graph(){
    this->incomingBuffer_ = new Buffer();
    this->outgoingBuffer_ = new Buffer();
    this->incomingIndex_ = new NodeIndex(this->incomingBuffer_);
    this->outgoingIndex_ = new NodeIndex(this->outgoingBuffer_);
}

bool Graph::add(uint32_t from, uint32_t to){
    bool suc1, suc2;
    if(this->incomingIndex_->getNeighborCount(to) < this->outgoingIndex_->getNeighborCount(from)){
        suc1 = this->addToPairWithDupCheck(this->incomingIndex_,
                                           this->incomingBuffer_,
                                           to,
                                           from);
        if(suc1){
            suc2 = this->addToPair(this->outgoingIndex_,
                                this->outgoingBuffer_,
                                from,
                                to);
        }
    }
    else{
        suc1 = this->addToPairWithDupCheck(this->outgoingIndex_,
                                           this->outgoingBuffer_,
                                           from,
                                           to);
        if(suc1){
            suc2 = this->addToPair(this->incomingIndex_,
                                   this->incomingBuffer_,
                                   to,
                                   from);
        }
    }
    //suc1 = this->addToPair(this->incomingIndex_, this->incomingBuffer_, to, from);
    //suc2 = this->addToPair(this->outgoingIndex_, this->outgoingBuffer_, from, to);
    return suc1 && suc2;
}

bool Graph::addToPair(NodeIndex* index, Buffer* buffer, uint32_t target, uint32_t node){
    ptr lNodePtr = index->getListTail(target);
    if(lNodePtr == PTR_NULL){
        // Node does not exist
        // We have to create it
        index->insertNode(target);
        lNodePtr = index->getListTail(target);
    }
    // Get actual list node from lNodePtr
    ListNode *listNode = buffer->getListNode(lNodePtr);
    // Add if we have space, else create a new list node
    if(listNode->getNeighborCount() < listNode->getNeighborMax()){
        listNode->addNeighbor(node);
    }
    else{
        ptr newAddr;
        newAddr = buffer->allocNewNode();
        listNode = buffer->getListNode(lNodePtr);

        //if(newAddr == PTR_NULL){
        //    return false; // We failed to allocate
        //}

        ListNode *newNode = buffer->getListNode(newAddr);

        listNode->setNextListNode(newAddr);
        listNode = buffer->getListNode(newAddr);
        listNode->addNeighbor(node);
        index->setListTail(target, newAddr);
    }
    index->incrementNeighbors(target);
    return true;
}


bool Graph::addToPairWithDupCheck(NodeIndex* index, Buffer* buffer, uint32_t target, uint32_t node){
    // Check if node exists
    ptr lNodePtr;
    lNodePtr = index->getListHead(target);

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

    // Increment neighbor count in index
    index->incrementNeighbors(target);
    //cout << target << "->" << index->getNeighborCount(target) << endl;

    // Skip if full
    while(listNode->getNeighborMax() == listNode->getNeighborCount()){

        // Check if next list node exists
        if(listNode->getNextListNode() == PTR_NULL){
            // We create the next list node
            ptr newAddr;
            newAddr = buffer->allocNewNode();
            listNode=buffer->getListNode(lNodePtr);

            if(newAddr == PTR_NULL){
                return false; // We failed to allocate
            }

            listNode->setNextListNode(newAddr);

            // Update list tail
            index->setListTail(target, newAddr);
        }

        lNodePtr=listNode->getNextListNode();
        listNode = buffer->getListNode(lNodePtr);

        // Check if neighbor already exists
        if(listNode->containsNeighbor(node)){
            return false; // Its already there
        }
    }
    listNode->addNeighbor(node);
    return true;
}

long Graph::query(uint32_t from, uint32_t to){

    // Validate nodes
    if(this->outgoingIndex_->getListHead(from) == PTR_NULL ||
       this->incomingIndex_->getListHead(to) == PTR_NULL){
        return -1;
    }

    Queue startQueue, endQueue;

    Hash<uint32_t> startVisited((this)->closedSetSize_);
    Hash<uint32_t> endVisited((this)->closedSetSize_);
    ResultCode resCodeStart, resCodeEnd;

    startQueue.enqueue(UINT_MAX-1);
    endQueue.enqueue(UINT_MAX-1);

    uint32_t startCurrentNode,endCurrentNode;
    long startCurrentLength,endCurrentLength;
    ListNode* startCurrentListNode;
    ListNode* endCurrentListNode;
    ptr startCurrentNodePtr,endCurrentNodePtr;
    uint32_t* startNodeNeighbors;
    uint32_t* endNodeNeighbors;
    uint32_t startNodeNeighborsNumber;
    uint32_t endNodeNeighborsNumber;
    uint32_t startCurrentNeighbors=0;
    uint32_t endCurrentNeighbors=0;

    NodeIndex* outgoingIndex = this->outgoingIndex_;
    NodeIndex* incomingIndex = this->incomingIndex_;

    startCurrentLength = 0;
    endCurrentLength = 0;
    startCurrentNode = from;
    endCurrentNode = to;

    while(!startQueue.isEmpty() && !endQueue.isEmpty()){

    	if(startCurrentNeighbors <= endCurrentNeighbors){
            startCurrentNeighbors = 0;

            while(startCurrentNode != UINT_MAX-1){

                //If current Node is already visited skip
                startVisited.get(startCurrentNode, resCodeStart);
                if(resCodeStart == NOT_FOUND){
                    startVisited.add(startCurrentNode, startCurrentNode);
                    startCurrentNodePtr=outgoingIndex->getListHead(startCurrentNode);
                    if(startCurrentNodePtr == PTR_NULL){
                        continue;
                    }
                    startCurrentListNode=outgoingBuffer_->getListNode(startCurrentNodePtr);

                    //Push node's neighbors that are not in closed set
                    while(true){
                        startNodeNeighbors=startCurrentListNode->getNeighborsPtr();
                        //For every neighbors inside the current list node
                        startNodeNeighborsNumber=startCurrentListNode->getNeighborCount();
                        for(uint32_t i = 0; i < startNodeNeighborsNumber; i++){
                            //If current neighbor is the target one:return
                            endVisited.get(startNodeNeighbors[i], resCodeEnd);
                            if(resCodeEnd == FOUND){
                                return startCurrentLength+endCurrentLength;
                            }
                            startQueue.enqueue(startNodeNeighbors[i]);
                            startCurrentNeighbors++;
                        }
                        //Get the next list node pointer from the current one
                        startCurrentNodePtr=startCurrentListNode->getNextListNode();
                        if(startCurrentNodePtr == PTR_NULL)
                            break;
                        else
                            startCurrentListNode=outgoingBuffer_->getListNode(startCurrentNodePtr);
                    }
                }
                startCurrentNode = startQueue.dequeue();
            }

            //Update for next level
            startCurrentLength++;
            if(startQueue.isEmpty())
                break;
            startQueue.enqueue(UINT_MAX-1);
            startCurrentNode=startQueue.dequeue();
    	}

        //End side implementation

    	else{
            endCurrentNeighbors = 0;

            while(endCurrentNode != UINT_MAX-1){

                //If current Node is already visited skip
                endVisited.get(endCurrentNode, resCodeEnd);
                if(resCodeEnd == NOT_FOUND){
                    endVisited.add(endCurrentNode, endCurrentNode);
                    endCurrentNodePtr=incomingIndex->getListHead(endCurrentNode);
                    if(endCurrentNodePtr == PTR_NULL){
                        continue;
                    }
                    endCurrentListNode=incomingBuffer_->getListNode(endCurrentNodePtr);

                    //Push node's neighbors that are not in closed set
                    while(true){
                        endNodeNeighbors = endCurrentListNode->getNeighborsPtr();
                        //For every neighbors inside the current list node
                        endNodeNeighborsNumber=endCurrentListNode->getNeighborCount();
                        for(uint32_t i = 0; i < endNodeNeighborsNumber; i++){
                            //If current neighbor is the target one:return
                            startVisited.get(endNodeNeighbors[i], resCodeStart);
                            if(resCodeStart == FOUND){
                                return startCurrentLength + endCurrentLength;
                            }
                            endQueue.enqueue(endNodeNeighbors[i]);
                            endCurrentNeighbors++;
                        }
                        //Get the next list node pointer from the current one
                        endCurrentNodePtr = endCurrentListNode->getNextListNode();
                        if(endCurrentNodePtr == PTR_NULL)
                            break;
                        else
                            endCurrentListNode = incomingBuffer_->getListNode(endCurrentNodePtr);
                    }
                }
                endCurrentNode = endQueue.dequeue();
            }

            //Update for next level
            endCurrentLength++;
            if(endQueue.isEmpty())
                break;
            endQueue.enqueue(UINT_MAX-1);
            endCurrentNode = endQueue.dequeue();
        }
    }
    return -1;
}

/*
bool Graph::addToPair(NodeIndex* index, Buffer* buffer, uint32_t target, uint32_t node, bool checkDuplicates){
    // Check if node exists
    ptr lNodePtr;
    if(checkDuplicates){
        lNodePtr = index->getListHead(target);
    }
    else{
        lNodePtr = index->getListTail(target);
    }

    if(lNodePtr == PTR_NULL){
        // Create node
        index->insertNode(target);
        lNodePtr = index->getListHead(target);
    }
    // Get actual list node
    ListNode* listNode = buffer->getListNode(lNodePtr);

    // Check if neighbor already exists
    if(checkDuplicates){
        if(listNode->containsNeighbor(node)){
            return true; // Its already there
        }
    }

    // Increment neighbor count in index
    index->incrementNeighbors(target);
    //cout << target << "->" << index->getNeighborCount(target) << endl;

    // Skip if full
    while(listNode->getNeighborMax() == listNode->getNeighborCount()){

        // Check if next list node exists
        if(listNode->getNextListNode() == PTR_NULL){
            // We create the next list node
            ptr newAddr;
            newAddr = buffer->allocNewNode();
            listNode=buffer->getListNode(lNodePtr);

            if(newAddr == PTR_NULL){
                return false; // We failed to allocate
            }

            listNode->setNextListNode(newAddr);

            // Update list tail
            index->setListTail(target, newAddr);
        }

        lNodePtr=listNode->getNextListNode();
        listNode = buffer->getListNode(lNodePtr);

        // Check if neighbor already exists
        if(checkDuplicates){
            if(listNode->containsNeighbor(node)){
                return true; // Its already there
            }
        }
    }
    listNode->addNeighbor(node);
    return true;
}
*/

Graph::~Graph(){
    delete incomingBuffer_;
    delete outgoingBuffer_;
    delete incomingIndex_;
    delete outgoingIndex_;
}
