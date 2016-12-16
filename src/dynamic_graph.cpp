#include "dynamic_graph.h"
#include "queue.h"

/****************************/
/*----------CC CODE---------*/
/****************************/

CC::CC(){
    lastComponent = 0;
    updateIndexSize_ = 0;
    ccindex_ = (int32_t*) malloc(ccLimit_ * sizeof(int32_t));
    memset(ccindex_, -1, ccLimit_ * sizeof(int32_t));
    ccSize_ = 0;
    ccMax_ = ccLimit_;
}

CC::~CC(){
    free(ccindex_);
}

void CC::addNodeToComponent(uint32_t nodeId, int32_t ccId){
    if(nodeId > ccMax_){
        int32_t *resized;
        uint32_t newSize = 2 * ccMax_;
        if(nodeId > newSize){
            newSize = nodeId;
        }
        resized = (int32_t *) realloc(ccindex_, sizeof(int32_t) * newSize); // Double size
        if(resized == NULL){
            std::cout << "Error allocating memory";
            return;
        }
        memset(resized + ccMax_, -1, (newSize - ccMax_) * sizeof(int32_t));
        ccMax_ = newSize;
        ccindex_ = resized;
        
    }
    ccindex_[nodeId] = ccId;
    ccSize_++;
}

void CC::postAddNodeToComponent(uint32_t nodeId, int32_t ccId){
    addNodeToComponent(nodeId, ccId);
    
    if(ccId > (int32_t) this->updateIndexSize_){
        std::cout << "Realloc update" << std::endl;
        int32_t *resized;
        uint32_t newSize = 2 * this->updateIndexSize_;
        if(ccId > (int32_t) newSize){
            newSize = ccId; // Double or use ccid; whichever bigger
        }
        resized = (int32_t*) realloc(this->updateIndex_, newSize * sizeof(int32_t));
        if(resized == NULL){
            std::cout << "Error allocating!" << std::endl;
            return;
        }
        this->updateIndexSize_ = newSize;
        this->updateIndex_ = resized;
    }
}

void CC::joinComponents(int32_t comp1, int32_t comp2){
    int32_t commonCC = updateIndex_[comp1];
    if(commonCC == -1){
        updateIndex_[comp1] = comp1;
        commonCC = comp1;
    }
    int32_t toBeMerged = updateIndex_[comp2];


    bool duplicateComp1 = false;
    bool duplicateComp2 = false;
    
    updateIndex_[comp2] = commonCC;
    if(toBeMerged != -1){
        for(uint32_t i = 0; i < mergedSize_; i++){
            if(updateIndex_[merged_[i]] == toBeMerged){
                updateIndex_[merged_[i]] = commonCC;
            }
            if(merged_[i] == comp1){
                duplicateComp1 = true;
            }
            else if(merged_[i] == comp2){
                duplicateComp2 = true;
            }
        }
    }

    if(mergedSize_ >= mergedMax_ - 1){
        int32_t *resized;
        uint32_t newSize = 2 * mergedMax_;
        resized = (int32_t*) realloc(merged_, sizeof(int32_t) * newSize);
        if(resized == NULL){
            std::cout << "Failure to allocate memory" << std::endl;
            return ;
        }
        merged_ = resized;
        mergedMax_ = newSize;
    }
    if(!duplicateComp1){
        merged_[mergedSize_] = comp1;
        mergedSize_++;
    }
    if(!duplicateComp2){
        merged_[mergedSize_] = comp2;
        mergedSize_++;
    }
    mergedHits += mergedSize_;
}

int32_t CC::findNodeConnectedComponentID(uint32_t nodeId){
    return ccindex_[nodeId];
}

bool CC::areConnected(int32_t comp1, int32_t comp2){
    if(comp1 == comp2) {
        return true;   
    }
    else if(comp1 == -1 || comp2 == -1){
        return false;
    }
    else if(updateIndex_[comp1] == -1 && updateIndex_[comp2] == -1){
        return false;
    }
    else if(updateIndex_[comp1] == updateIndex_[comp2]){
        return true;
    }
    else{
        return false;
    }
}

void CC::initUpdateIndex(uint32_t size){
    updateIndexSize_ = lastComponent + updateMargin_;
    updateIndex_ = (int32_t *) malloc(sizeof(int32_t) * updateIndexSize_);
    memset(updateIndex_, -1, sizeof(int32_t) * updateIndexSize_);
    merged_ = (int32_t *) malloc(sizeof(int32_t) * mergedInitial_);
    mergedSize_ = 0;
    mergedMax_ = mergedInitial_;
    mergedHits = 0;
}

uint32_t CC::getComponentCount(){
    return ccSize_;
}

uint32_t CC::getMergedSize(){
    return mergedSize_;
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

        if(cfrom == -1 && cto == -1){ // Case both do not exist
            // Create them both and add them to the same CC
            this->connectedComponents_->lastComponent++;
            this->connectedComponents_->postAddNodeToComponent(from, this->connectedComponents_->lastComponent);
            cfrom = this->connectedComponents_->findNodeConnectedComponentID(from);
            this->connectedComponents_->postAddNodeToComponent(to, this->connectedComponents_->lastComponent);
            cto = this->connectedComponents_->findNodeConnectedComponentID(to);
        }
        else{
            if(cfrom == -1){ // Only from exists
                // Create it and put it on to CC
                this->connectedComponents_->postAddNodeToComponent(from, this->connectedComponents_->findNodeConnectedComponentID(to));
            }
            else if(cto == -1){ // Only to exists
                // Create it and put it on from CC
                this->connectedComponents_->postAddNodeToComponent(to, this->connectedComponents_->findNodeConnectedComponentID(from));
            }
            else{
                // Both CCs exist
                // Join them if not already connected
                if(!this->connectedComponents_->areConnected(cfrom, cto)){
                    this->connectedComponents_->joinComponents(cfrom, cto);
                }
            }
        }
        return true;
    }
    else{
        return false;
    }
}

CC* DynamicGraph::getCC(){
    return this->connectedComponents_;
}

long DynamicGraph::query(uint32_t from, uint32_t to){
    int32_t comp1, comp2;
    comp1 = this->connectedComponents_->findNodeConnectedComponentID(from);
    comp2 = this->connectedComponents_->findNodeConnectedComponentID(to);

    if(this->connectedComponents_->areConnected(comp1, comp2)){
        return Graph::query(from, to);
    }
    // If they dont belong in the same CC, we know no connection exists
    return -1;
}

/****************************/
/*---END OF DYNAMIC GRAPH---*/
/****************************/
