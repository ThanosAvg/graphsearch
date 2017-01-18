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
    this->startVisited = NULL;
    this->endVisited = NULL;
    this->startVisitedKey = 0;
    this->endVisitedKey = 0;
    this->startVisitedSize = 0;
    this->endVisitedSize = 0;
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
        return false; // Its already there
    }

    // Skip if full
    uint32_t neighborMaxNumber=listNode->getNeighborMax();

    while(neighborMaxNumber == listNode->getNeighborCount()){

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
            listNode=buffer->getListNode(newAddr);
            break;
        }

        lNodePtr=listNode->getNextListNode();
        listNode = buffer->getListNode(lNodePtr);

        // Check if neighbor already exists
        if(listNode->containsNeighbor(node)){
            // Its already there
            return false;
        }
    }
    listNode->addNeighbor(node);
    index->incrementNeighbors(target);
    return true;
}

bool Graph::expandLevel(NodeIndex* index, Buffer* buffer, Queue* queue, uint32_t myVisitedKey,
    uint32_t* myVisited, uint32_t targetVisitedKey, uint32_t* targetVisited, uint32_t& currentNeighbors){
    // Expands the current nodes in the queue(one level).
    // Returns true if the desired path is found,false if not.

    ResultCode resCode;
    ptr currentNodePtr;
    ListNode* currentListNode;
    uint32_t* nodeNeighbors;
    uint32_t nodeNeighborsNumber;

    uint32_t currentNode = queue->dequeue();

    while(currentNode != LEVEL_END){
        //If current Node is already visited skip
        if(myVisited[currentNode]!=myVisitedKey){
            //If it's not visited mark it as visited
            myVisited[currentNode]=myVisitedKey;
            currentNodePtr = index->getListHead(currentNode);
            if(currentNodePtr == PTR_NULL){
                continue;
            }
            currentListNode = buffer->getListNode(currentNodePtr);
            currentNeighbors += index->getNeighborCount(currentNode);

            //Push node's neighbors that are not in closed set
            while(true){
                nodeNeighbors = currentListNode->getNeighborsPtr();
                //For every neighbors inside the current list node
                nodeNeighborsNumber = currentListNode->getNeighborCount();
                for(uint32_t i = 0; i < nodeNeighborsNumber; i++){
                    //If current neighbor has been found on the other side:return
                    if(targetVisited[nodeNeighbors[i]]==targetVisitedKey){
                        return true;
                    }
                    queue->enqueue(nodeNeighbors[i]);
                }
                //Get the next list node pointer from the current one
                currentNodePtr = currentListNode->getNextListNode();
                if(currentNodePtr == PTR_NULL)
                    break;
                else
                    currentListNode = buffer->getListNode(currentNodePtr);
            }
        }
        currentNode = queue->dequeue();
    }
    //Target node not found
    return false;
}

long Graph::query(uint32_t from, uint32_t to){
    // Finds and returns the path distance from the source node to the target node.
    // Returns -1 if a paths does not exist.

    if(from==to)
        return 0;

    // Validate nodes
    if(this->outgoingIndex_->getListHead(from) == PTR_NULL ||
       this->incomingIndex_->getListHead(to) == PTR_NULL){
        return -1;
    }

    //Create visited arrays if not created
    if(this->startVisited==NULL){
        this->startVisitedSize=this->outgoingIndex_->getCurrentSize();
        this->startVisited=(uint32_t*)calloc(this->startVisitedSize,sizeof(uint32_t));
    }
    //else if(this->startVisitedSize<this->outgoingIndex_->getCurrentSize())
        //cout << "startVistedSize Increased" << endl;
    if(this->endVisited==NULL){
        this->endVisitedSize=this->incomingIndex_->getCurrentSize();
        this->endVisited=(uint32_t*)calloc(this->endVisitedSize,sizeof(uint32_t));
    }
    //else if(this->endVisitedSize<this->incomingIndex_->getCurrentSize())
        //cout << "endVistedSize Increased" << endl;

    //Check if maximum key has been reached and reset the visited arrays
    if(this->startVisitedKey==UINT_MAX){
        memset(this->startVisited,0,this->outgoingIndex_->getCurrentSize());
        this->startVisitedKey=0;
    }
    if(this->endVisitedKey==UINT_MAX){
        memset(this->endVisited,0,this->incomingIndex_->getCurrentSize());
        this->endVisitedKey=0;
    }
    //Increase the visited key of the current search
    this->startVisitedKey++;
    this->endVisitedKey++;

    //Create two queues for each side for bi-bfs
    Queue startQueue, endQueue;

    //Push into the queues,the initial nodes for each side(level 0)
    startQueue.enqueue(from);
    endQueue.enqueue(to);
    startQueue.enqueue(LEVEL_END);
    endQueue.enqueue(LEVEL_END);

    //Current length measures the depth of search(levels) in each side
    long startCurrentLength,endCurrentLength;
    uint32_t startCurrentNeighbors=0;
    uint32_t endCurrentNeighbors=0;
    startCurrentLength = 0;
    endCurrentLength = 0;

    while(true){
        // Run bidirectional bfs for source and target node

        //Choose only the side with less neighbors in its queue
        if(startCurrentNeighbors <= endCurrentNeighbors){
            startCurrentNeighbors = 0;
            //Expand the nodes currently in queue(one level),and add the next level nodes
            if(expandLevel(this->outgoingIndex_,this->outgoingBuffer_,&startQueue,this->startVisitedKey,
                this->startVisited,this->endVisitedKey,this->endVisited,startCurrentNeighbors)==true)
                return startCurrentLength+endCurrentLength;
            startCurrentLength++;
            if(startQueue.isEmpty())
                return -1;
            startQueue.enqueue(LEVEL_END);
        }
        else{
            endCurrentNeighbors = 0;
            if(expandLevel(this->incomingIndex_,this->incomingBuffer_,&endQueue,this->endVisitedKey,
                this->endVisited,this->startVisitedKey,this->startVisited,endCurrentNeighbors)==true)
                return startCurrentLength+endCurrentLength;
            endCurrentLength++;
            if(endQueue.isEmpty())
                return -1;
            endQueue.enqueue(LEVEL_END);
        }

    }

    return -1;
}

uint32_t Graph::getOutgoingIndexSize(){
    return this->outgoingIndex_->getCurrentSize();
}

uint32_t Graph::getIncomingIndexSize(){
    return this->incomingIndex_->getCurrentSize();
}

Graph::~Graph(){
    delete incomingBuffer_;
    delete outgoingBuffer_;
    delete incomingIndex_;
    delete outgoingIndex_;
    if(startVisited!=NULL)
        free(startVisited);
    if(endVisited!=NULL)
        free(endVisited);
}
