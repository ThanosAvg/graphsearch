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
    suc1 = this->addToPair(this->incomingIndex_, this->incomingBuffer_, to, from);
    suc2 = this->addToPair(this->outgoingIndex_, this->outgoingBuffer_, from, to);
    return suc1 && suc2;
}

long Graph::query(uint32_t from, uint32_t to){
    if(from>=this->outgoingIndex_->getCurrentSize() || to>=this->outgoingIndex_->getCurrentSize())
        return -1;
    Queue startQueue;
    bool* startVisited=(bool*)malloc(this->outgoingIndex_->getCurrentSize()*sizeof(bool));
    cout << "CURRENT LENGTH OUTGOIND:" << this->outgoingIndex_->getCurrentSize() << endl;
    cout << "CURRENT LENGTH INGOING:" << this->incomingIndex_->getCurrentSize() << endl;
    for(int i=0;i<this->outgoingIndex_->getCurrentSize();i++)
        startVisited[i]=false;
    Queue endQueue;

    startQueue.enqueue(from);
    startQueue.enqueue(UINT_MAX-1);
    endQueue.enqueue(to);

    uint32_t currentNode;
    long currentLength=0;
    NodeIndex* outgoingIndex=this->outgoingIndex_;
    ListNode* currentListNode;
    ptr currentNodePtr;
    uint32_t* nodeNeighbors;
    while(!startQueue.isEmpty()){
        //cout << "OK1" << endl;
        currentNode=startQueue.dequeue();
        //cout << "OK2" << endl;
        if(currentNode==UINT_MAX-1){
            currentLength++;
            if(startQueue.isEmpty())
                break;
            startQueue.enqueue(UINT_MAX-1);
            currentNode=startQueue.dequeue();
        }
        //cout << "OK3" << endl;
        //cout << "CURRENT NODE:" << currentNode << endl;
        if(startVisited[currentNode]==false){
            //cout << "OK3" << endl;
            startVisited[currentNode]=true;
            currentNodePtr=outgoingIndex->getListHead(currentNode);
            if(currentNodePtr==PTR_NULL){
                //cout << "uknown node:" << currentNode << endl;
                continue;
            }
            currentListNode=outgoingBuffer_->getListNode(currentNodePtr);
            //cout << "OK4" << endl;
            while(true){
                nodeNeighbors=currentListNode->getNeighborsPtr();
                for(int i=0;i<currentListNode->getNeighborCount();i++){
                    //cout << "neighbor:" << i << "->" << nodeNeighbors[i] << endl;
                    if(nodeNeighbors[i]==to){
                        free(startVisited);
                        return currentLength+1;
                    }
                    if(nodeNeighbors[i]<0 || nodeNeighbors[i]>1000000)
                        cout << "HEREEEEEEEEEEEEEEEEEEEEEEEEEEE0000:" << nodeNeighbors[i] << endl;
                    startQueue.enqueue(nodeNeighbors[i]);
                }
                currentNodePtr=currentListNode->getNextListNode();
                if(currentNodePtr==PTR_NULL)
                    break;
                else
                    currentListNode=outgoingBuffer_->getListNode(currentNodePtr);
            }
            //cout << "OK5" << endl;
        }
    }
    //cout << "OK4" << endl;
    free(startVisited);
    //cout << "OK5" << endl;
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
