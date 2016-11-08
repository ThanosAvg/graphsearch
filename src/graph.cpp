#include "graph.h"
#include "queue.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <limits.h>

using namespace std;

Graph::Graph(){
    this->incomingBuffer_ = new Buffer();
    this->outgoingBuffer_ = new Buffer();
    this->incomingIndex_ = new NodeIndex(this->incomingBuffer_);
    this->outgoingIndex_ = new NodeIndex(this->outgoingBuffer_);
}

bool Graph::add(uint32_t from, uint32_t to){
    bool suc1, suc2;
    bool check1 = true, check2 = true;
    if(this->incomingIndex_->getNeighborCount(to) < this->outgoingIndex_->getNeighborCount(from)){
        check1 = true;
        check2 = false;
    }
    else{
        check2 = true;
        check1 = false;
    }
    suc1 = this->addToPair(this->incomingIndex_, this->incomingBuffer_, to, from, check1);
    suc2 = this->addToPair(this->outgoingIndex_, this->outgoingBuffer_, from, to, check2);
    return suc1 && suc2;
}

long Graph::query(uint32_t from, uint32_t to){

    // Validate nodes
    if(this->outgoingIndex_->getListHead(from) == PTR_NULL ||
       this->incomingIndex_->getListHead(to) == PTR_NULL){
        return -1;
    }

    Queue startQueue, endQueue;

    Hash<uint32_t>* startVisited = new Hash<uint32_t>((this)->closedSetSize_);
    Hash<uint32_t>* endVisited = new Hash<uint32_t>((this)->closedSetSize_);
    ResultCode resCodeStart, resCodeEnd;

    startQueue.enqueue(from);
    startQueue.enqueue(UINT_MAX-1);

    endQueue.enqueue(to);
    endQueue.enqueue(UINT_MAX-1);

    uint32_t startCurrentNode,endCurrentNode;
    long startCurrentLength,endCurrentLength;
    ListNode* startCurrentListNode;
    ListNode* endCurrentListNode;
    ptr startCurrentNodePtr,endCurrentNodePtr;
    uint32_t* startNodeNeighbors;
    uint32_t* endNodeNeighbors;
    uint32_t startCurrentNeighbors=0;
    uint32_t endCurrentNeighbors=0;

    NodeIndex* outgoingIndex = this->outgoingIndex_;
    NodeIndex* incomingIndex = this->incomingIndex_;

    startCurrentLength = 0;
    endCurrentLength = 0;
    startCurrentNode = startQueue.dequeue();
    endCurrentNode = endQueue.dequeue();

    while(!startQueue.isEmpty() && !endQueue.isEmpty()){

    	if(startCurrentNeighbors <= endCurrentNeighbors){
            startCurrentNeighbors = 0;

            while(startCurrentNode != UINT_MAX-1){

                //If current node is not visited
                startVisited->get(startCurrentNode, resCodeStart);
                if(resCodeStart == NOT_FOUND){
                    startVisited->add(startCurrentNode, startCurrentNode);
                    startCurrentNodePtr=outgoingIndex->getListHead(startCurrentNode);
                    if(startCurrentNodePtr == PTR_NULL){
                        continue;
                    }
                    startCurrentListNode=outgoingBuffer_->getListNode(startCurrentNodePtr);

                    //Push node's neighbors that are not in closed set
                    while(true){
                        startNodeNeighbors=startCurrentListNode->getNeighborsPtr();
                        //For every neighbors inside the current list node
                        for(uint32_t i = 0; i < startCurrentListNode->getNeighborCount(); i++){
                            //If current neighbor is the target one:return
                            endVisited->get(startNodeNeighbors[i], resCodeEnd);
                            if(resCodeEnd == FOUND){
                                delete startVisited;
                                delete endVisited;
                                return startCurrentLength+endCurrentLength;
                            }
                            //If current neighbor is already in closed set do not push him
                            startVisited->get(startNodeNeighbors[i], resCodeStart);
                            if(resCodeStart == NOT_FOUND){
                                startQueue.enqueue(startNodeNeighbors[i]);
                                startCurrentNeighbors++;
                            }
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

    	if(endCurrentNeighbors < startCurrentNeighbors){
            endCurrentNeighbors = 0;

            while(endCurrentNode != UINT_MAX-1){

                //If current node is not visited
                endVisited->get(endCurrentNode, resCodeEnd);
                if(resCodeEnd == NOT_FOUND){
                    endVisited->add(endCurrentNode, endCurrentNode);
                    endCurrentNodePtr=incomingIndex->getListHead(endCurrentNode);
                    if(endCurrentNodePtr == PTR_NULL){
                        continue;
                    }
                    endCurrentListNode=incomingBuffer_->getListNode(endCurrentNodePtr);

                    //Push node's neighbors that are not in closed set
                    while(true){
                        endNodeNeighbors = endCurrentListNode->getNeighborsPtr();
                        //For every neighbors inside the current list node
                        for(uint32_t i = 0; i < endCurrentListNode->getNeighborCount(); i++){
                            //If current neighbor is the target one:return
                            startVisited->get(endNodeNeighbors[i], resCodeStart);
                            if(resCodeStart == FOUND){
                                delete startVisited;
                                delete endVisited;
                                return startCurrentLength + endCurrentLength;
                            }
                            //If current neighbor is already in closed set do not push him
                            endVisited->get(endNodeNeighbors[i], resCodeEnd);
                            if(resCodeEnd == NOT_FOUND){
                                endQueue.enqueue(endNodeNeighbors[i]);
                                endCurrentNeighbors++;
                            }
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
    delete startVisited;
    delete endVisited;
    return -1;
}

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

Graph::~Graph(){
    delete incomingBuffer_;
    delete outgoingBuffer_;
    delete incomingIndex_;
    delete outgoingIndex_;
}
