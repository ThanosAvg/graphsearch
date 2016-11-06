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

    uint32_t outIndexSize=this->outgoingIndex_->getMaxSize();
    uint32_t inIndexSize=this->incomingIndex_->getMaxSize();
    //Check if start/target node does not exist
    if(from>=outIndexSize || to>=inIndexSize)
        return -1;

    Queue startQueue,endQueue;
    bool* startVisited=new bool[outIndexSize];
    std::fill(startVisited, startVisited+outIndexSize, false);
    bool* endVisited=new bool[inIndexSize];
    std::fill(endVisited, endVisited+inIndexSize, false);

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

    NodeIndex* outgoingIndex=this->outgoingIndex_;
    NodeIndex* incomingIndex=this->incomingIndex_;

    startCurrentLength=0;
    endCurrentLength=0;
    startCurrentNode=startQueue.dequeue();
    endCurrentNode=endQueue.dequeue();
    //outgoingIndex=&(this->outgoingIndex_);
    //incomingIndex=&(this->incomingIndex_);

    while(!startQueue.isEmpty() && !endQueue.isEmpty()){

        while(startCurrentNode!=UINT_MAX-1){
    		
    		//If current node is not visited
            if(startVisited[startCurrentNode]==false){
                //cout << "CURNODE:" << currentNode << endl;
                startVisited[startCurrentNode]=true;
                startCurrentNodePtr=outgoingIndex->getListHead(startCurrentNode);
                if(startCurrentNodePtr==PTR_NULL){
                    continue;
                }
                startCurrentListNode=outgoingBuffer_->getListNode(startCurrentNodePtr);
                
                //Push node's neighbors that are not in closed set
                while(true){
                    startNodeNeighbors=startCurrentListNode->getNeighborsPtr();
                    //For every neighbors inside the current list node
                    for(int i=0;i<startCurrentListNode->getNeighborCount();i++){
                        //If current neighbor is the target one:return
                        if(endVisited[startNodeNeighbors[i]]){
                            delete[] startVisited;
                            delete[] endVisited;
                            return startCurrentLength+endCurrentLength;
                        }
                        //If current neighbor is already in closed set do not push him 
                        if(startVisited[startNodeNeighbors[i]]==false)
                            startQueue.enqueue(startNodeNeighbors[i]);
                    }
                    //Get the next list node pointer from the current one
                    startCurrentNodePtr=startCurrentListNode->getNextListNode();
                    if(startCurrentNodePtr==PTR_NULL)
                        break;
                    else
                        startCurrentListNode=outgoingBuffer_->getListNode(startCurrentNodePtr);
                }
            }
            startCurrentNode=startQueue.dequeue();
        }

        //Update for next level
        startCurrentLength++;
        if(startQueue.isEmpty())
            break;
        startQueue.enqueue(UINT_MAX-1);
        startCurrentNode=startQueue.dequeue();

        //*****CODE FOR END SIDE****

        while(endCurrentNode!=UINT_MAX-1){
    		
    		//If current node is not visited
            if(endVisited[endCurrentNode]==false){
                //cout << "CURNODE:" << currentNode << endl;
                endVisited[endCurrentNode]=true;
                endCurrentNodePtr=incomingIndex->getListHead(endCurrentNode);
                if(endCurrentNodePtr==PTR_NULL){
                    continue;
                }
                endCurrentListNode=incomingBuffer_->getListNode(endCurrentNodePtr);
                
                //Push node's neighbors that are not in closed set
                while(true){
                    endNodeNeighbors=endCurrentListNode->getNeighborsPtr();
                    //For every neighbors inside the current list node
                    for(int i=0;i<endCurrentListNode->getNeighborCount();i++){
                        //If current neighbor is the target one:return
                        if(startVisited[endNodeNeighbors[i]]){
                            delete[] startVisited;
                            delete[] endVisited;
                            return startCurrentLength+endCurrentLength;
                        }
                        //If current neighbor is already in closed set do not push him 
                        if(endVisited[endNodeNeighbors[i]]==false)
                            endQueue.enqueue(endNodeNeighbors[i]);
                    }
                    //Get the next list node pointer from the current one
                    endCurrentNodePtr=endCurrentListNode->getNextListNode();
                    if(endCurrentNodePtr==PTR_NULL)
                        break;
                    else
                        endCurrentListNode=incomingBuffer_->getListNode(endCurrentNodePtr);
                }
            }
            endCurrentNode=endQueue.dequeue();
        }

        //Update for next level
        endCurrentLength++;
        if(endQueue.isEmpty())
            break;
        endQueue.enqueue(UINT_MAX-1);
        endCurrentNode=endQueue.dequeue();

    }
    delete[] startVisited;
    delete[] endVisited;
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
