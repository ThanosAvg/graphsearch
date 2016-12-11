#include "dynamic_graph.h"
#include "queue.h"

/****************************/
/*----------CC CODE---------*/
/****************************/

CC::CC(){ //: ccindex_(bucketNIndex_){
    lastComponent = 0;
    updateIndexSize_ = 0;
    ccindex_ = (uint32_t*) malloc(ccLimit_ * sizeof(uint32_t));
    ccSize_ = 0;
    ccMax_ = ccLimit_;
}

CC::~CC(){
    free(ccindex_);
}

void CC::addNodeToComponent(uint32_t nodeId, uint32_t ccId){
    //this->ccindex_.add(ccId, nodeId);
    ccindex_[nodeId] = ccId;
    ccSize_++;
}

void CC::postAddNodeToComponent(uint32_t nodeId, uint32_t ccId){
//    std::cout << "Adding node to component" << std::endl;
    //this->ccindex_.add(ccId, nodeId);
    ccindex_[nodeId] = ccId;
    ccSize_++;
    /*
    if(ccId > this->updateIndexSize_){
        std::cout << "Realoc update" << std::endl;
        uint32_t *resized;
        uint32_t newSize = 2*this->updateIndexSize_;
        if(ccId > newSize){
            newSize = ccId;
        }
        resized = (uint32_t*) realloc(this->updateIndex_, newSize * sizeof(uint32_t));
        if(resized == NULL){
            std::cout << "Error allocating!" << std::endl;
            return;
        }
        memset(resized + this->updateIndexSize_, 0, (newSize - updateIndexSize_) * sizeof(uint32_t));
        this->updateIndexSize_ = newSize;
        this->updateIndex_ = resized;
    }
    */
}

void CC::joinComponents(uint32_t comp1, uint32_t comp2){
//    std::cout << "Joining" << std::endl;
    if(updateIndex_[comp1].flag == 0){
        updateIndex_[comp1].connected = (uint32_t*) malloc(this->updateLimit_ * sizeof(ConnectData));
        updateIndex_[comp1].flag = 1;
        updateIndex_[comp1].size = 0;
        updateIndex_[comp1].max = this->updateLimit_;
    }
    if(updateIndex_[comp2].flag == 0){
        updateIndex_[comp2].connected = (uint32_t*) malloc(this->updateLimit_ * sizeof(ConnectData));
        updateIndex_[comp2].flag = 1;
        updateIndex_[comp2].size = 0;
        updateIndex_[comp2].max = this->updateLimit_;
    }

    updateIndex_[comp1].connected[updateIndex_[comp1].size] = comp2;
    updateIndex_[comp1].size++;

    updateIndex_[comp2].connected[updateIndex_[comp2].size] = comp1;
    updateIndex_[comp2].size++;
}

int32_t CC::findNodeConnectedComponentID(uint32_t nodeId){
//    std::cout << "Finding NCCID" << std::endl;
    /*ResultCode rescode;
    int32_t ccid;
    ccid = ccindex_.get(nodeId, rescode);
    if(rescode == NOT_FOUND){
        return -1;
    }
    return ccid;*/
    return ccindex_[nodeId];
}

bool CC::areConnected(uint32_t comp1, uint32_t comp2){
//    std::cout << "Searching" << std::endl;
    Queue fringe;
    ResultCode rescode;
    Hash<uint32_t> visited(101);
    fringe.enqueue(comp1);
    uint32_t current;
    int depth = 0;
    while(!fringe.isEmpty()){
        current = fringe.dequeue();
        depth ++;
        //if(depth == 100) break;
        for(uint32_t i = 0; i < updateIndex_[current].size; i++){
            if(updateIndex_[current].connected[i] == comp2){
                //std::cout << "Connected ";
                //std::cout << "SUCCESS DEPTH=" << depth << std::endl;
                return true;
            }
            else if(updateIndex_[current].connected[i] != comp1){
                visited.get(updateIndex_[current].connected[i], rescode);
                if(rescode == NOT_FOUND){
                    fringe.enqueue(updateIndex_[current].connected[i]);
                    visited.add(updateIndex_[current].connected[i], updateIndex_[current].connected[i]);
                }
            }
        }
    }
    //std::cout << "FAILED DEPTH=" << depth << std::endl;
    return false;
}

void CC::initUpdateIndex(uint32_t size){
//    std::cout << "init update index" << std::endl;
    this->updateIndexSize_ = 4 * size;
    this->updateIndex_ = (ConnectData*) malloc(sizeof(ConnectData) *  this->updateIndexSize_);
    memset(this->updateIndex_, 0, sizeof(ConnectData) * this->updateIndexSize_);
}

uint32_t CC::getComponentCount(){
    return ccSize_;
}

/****************************/
/*--------END OF CC---------*/
/****************************/



/****************************/
/*------DYNAMIC GRAPH-------*/
/****************************/

DynamicGraph::DynamicGraph() : visited_(visitedSize_){
    // Create a CC
    this->connectedComponents_ = new CC();
    //this->estimateConnectedComponents();
}

DynamicGraph::~DynamicGraph(){
    //
}

void DynamicGraph::estimateConnectedComponents(){
    // Start iteration from each unvisited node
    // Travel using both incoming and outgoing edges
    Queue fringe;
    visited_.reset();
    NodeIndex *incoming = this->incomingIndex_;
    NodeIndex *outgoing = this->outgoingIndex_;
    Buffer *inBuffer = this->incomingBuffer_;
    Buffer *outBuffer = this->outgoingBuffer_;

    bool error;
    uint32_t node;
    ResultCode visitedcode;
    incoming->resetCursor();
    uint32_t currentComponent = -1;
    while(true){ // Stops when all nodes of index are checked
        node = incoming->getNextId(error);
        if(error) break;
        visited_.get(node, visitedcode);
        if(visitedcode == FOUND){
            continue; // Already visited, move along
        }

        // Run bfs and keep track of neighbors
        uint32_t currentNode;
        ptr currentNodeInPtr, currentNodeOutPtr;
        ListNode *currentListNode;
        uint32_t *currentNeighbors;
        fringe.enqueue(node);
        currentComponent++;
        while(!fringe.isEmpty()){
            currentNode = fringe.dequeue();
            currentNodeInPtr = incoming->getListHead(currentNode);
            currentNodeOutPtr = outgoing->getListHead(currentNode);

            if(currentNodeInPtr != PTR_NULL){
                currentListNode = inBuffer->getListNode(currentNodeInPtr);
                while(true){ // Till we have seen all neighbors
                    currentNeighbors = currentListNode->getNeighborsPtr();
                    for(uint32_t i = 0; i < currentListNode->getNeighborCount(); i++){
                        // Check if node is already visited
                        visited_.get(currentNeighbors[i], visitedcode);
                        if(visitedcode == NOT_FOUND){
                            //std::cout << "Found: " << currentNeighbors[i] << std::endl;
                            fringe.enqueue(currentNeighbors[i]);
                            visited_.add(currentNeighbors[i], currentNeighbors[i]);
                            this->connectedComponents_->addNodeToComponent(currentNeighbors[i], currentComponent);
                            // if(currentNeighbors[i] == 449448){
                            //     std::cout << "1.Found node in question, CC:" << currentComponent << std::endl;
                            //     getchar();
                            
                            //     std::cout << this->connectedComponents_->findNodeConnectedComponentID(449448) << std::endl;
                            // }
                        }
                    }
                    // Continue iterating
                    currentNodeInPtr = currentListNode->getNextListNode();
                    if(currentNodeInPtr == PTR_NULL){
                        break; // No more neighbors
                    }
                    else{
                        currentListNode = inBuffer->getListNode(currentNodeInPtr);
                    }
                }
            }

            if(currentNodeOutPtr != PTR_NULL){
                currentListNode = outBuffer->getListNode(currentNodeOutPtr);
                while(true){ // Till we have seen all neighbors
                    currentNeighbors = currentListNode->getNeighborsPtr();
                    for(uint32_t i = 0; i < currentListNode->getNeighborCount(); i++){
                        // Check if node is already visited
                        visited_.get(currentNeighbors[i], visitedcode);
                        if(visitedcode == NOT_FOUND){
                            // if(currentNeighbors[i] == 449448){
                            //     std::cout << "2.Found node in question, CC:" << currentComponent << std::endl;
                            //     getchar();
                            // }
                            //std::cout << "Found: " << currentNeighbors[i] << std::endl;
                            fringe.enqueue(currentNeighbors[i]);
                            visited_.add(currentNeighbors[i], currentNeighbors[i]);
                            this->connectedComponents_->addNodeToComponent(currentNeighbors[i], currentComponent);
                        }
                    }
                    // Continue iterating
                    currentNodeOutPtr = currentListNode->getNextListNode();
                    if(currentNodeOutPtr == PTR_NULL){
                        break; // No more neighbors
                    }
                    else{
                        currentListNode = outBuffer->getListNode(currentNodeOutPtr);
                    }
                }
            }
        }
    }
    this->connectedComponents_->lastComponent = currentComponent;
    this->connectedComponents_->initUpdateIndex(currentComponent);
}

bool DynamicGraph::postAdd(uint32_t from, uint32_t to){
    bool added = this->add(from, to);
    if(added){
        int32_t cfrom, cto;
        cfrom = this->connectedComponents_->findNodeConnectedComponentID(from);
        cto = this->connectedComponents_->findNodeConnectedComponentID(to);
        if(cfrom != cto && cfrom != -1 && cto != -1){
            this->connectedComponents_->joinComponents(cfrom, cto);
            return true;
        }

        if(cfrom == -1){
            this->connectedComponents_->lastComponent++;
            this->connectedComponents_->postAddNodeToComponent(from, this->connectedComponents_->lastComponent);
            cfrom = this->connectedComponents_->findNodeConnectedComponentID(from);
        }

        if(cto == -1){
            this->connectedComponents_->lastComponent++;
            this->connectedComponents_->postAddNodeToComponent(to, this->connectedComponents_->lastComponent);            
            cto = this->connectedComponents_->findNodeConnectedComponentID(to);
        }

        this->connectedComponents_->joinComponents(cfrom, cto);
    }
    else{
        std::cout << "NOT ADDED" << std::endl;
    }
}

CC* DynamicGraph::getCC(){
    return this->connectedComponents_;
}

long DynamicGraph::query(uint32_t from, uint32_t to){
    int32_t comp1, comp2;
    comp1 = this->connectedComponents_->findNodeConnectedComponentID(from);
    comp2 = this->connectedComponents_->findNodeConnectedComponentID(to);

    if(comp1 == -1 || comp2 == -1){
        //TODO: CHECK!
        //std::cout << "Hell Nodes" << from << " " << to << " CC:" << comp1 << " " << comp2 << " ";
        return -1;
    }
    if(comp1 == comp2){
        // Both are in the same CC, searching
        return Graph::query(from, to);
    }
    else if(this->connectedComponents_->areConnected(comp1, comp2)){
        return Graph::query(from, to);
    }
    // If they dont belong in the same CC, we know no connection exists
    //std::cout << "Not Connected";
    return -1;
}

/****************************/
/*---END OF DYNAMIC GRAPH---*/
/****************************/
