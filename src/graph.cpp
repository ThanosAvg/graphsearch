#include "graph.h"
#include "queue.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <queue>
#include <list>
#include <algorithm>

using namespace std;

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

    if(from>=this->outgoingIndex_->getMaxSize() || to>=this->outgoingIndex_->getMaxSize())
        return -1;

    Queue startQueue;
    bool* startVisited=new bool[this->outgoingIndex_->getMaxSize()];
    for(int i=0;i<this->outgoingIndex_->getMaxSize();i++)
        startVisited[i]=false;

    startQueue.enqueue(from);
    startQueue.enqueue(UINT_MAX-1);

    uint32_t currentNode;
    long currentLength=0;
    NodeIndex* outgoingIndex=this->outgoingIndex_;
    ListNode* currentListNode;
    ptr currentNodePtr;
    uint32_t* nodeNeighbors;

    while(!startQueue.isEmpty()){
        currentNode=startQueue.dequeue();

		//Check if we enter next level
        if(currentNode==UINT_MAX-1){
            currentLength++;
            if(startQueue.isEmpty())
                break;
            startQueue.enqueue(UINT_MAX-1);
            currentNode=startQueue.dequeue();
        }
		
		//If current node is not visited
        if(startVisited[currentNode]==false){
            //cout << "CURNODE:" << currentNode << endl;
            startVisited[currentNode]=true;
            currentNodePtr=outgoingIndex->getListHead(currentNode);
            if(currentNodePtr==PTR_NULL){
                continue;
            }
            currentListNode=outgoingBuffer_->getListNode(currentNodePtr);
            
            //Push node's neighbors that are not in closed set
            while(true){
                nodeNeighbors=currentListNode->getNeighborsPtr();
                //For everu neighbors inside the current list node
                for(int i=0;i<currentListNode->getNeighborCount();i++){
                    //If current neighbor is the target one:return
                    if(nodeNeighbors[i]==to){
                        delete startVisited;
                        return currentLength+1;
                    }
                    //If current neighbor is already in closed set do not push him 
                    if(startVisited[nodeNeighbors[i]]==false)
                        startQueue.enqueue(nodeNeighbors[i]);
                }
                //Get the next list node pointer from the current one
                currentNodePtr=currentListNode->getNextListNode();
                if(currentNodePtr==PTR_NULL)
                    break;
                else
                    currentListNode=outgoingBuffer_->getListNode(currentNodePtr);
            }
        }
    }
    delete startVisited;
    return -1;
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
            listNode=buffer->getListNode(lNodePtr);

            if(newAddr == PTR_NULL){
                return false; // We failed to allocate
            }

            listNode->setNextListNode(newAddr);
        }
        lNodePtr=listNode->getNextListNode();
        listNode = buffer->getListNode(lNodePtr);

        // Check if neighbor already exists
        if(listNode->containsNeighbor(node)){
            return true; // Its already there
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
