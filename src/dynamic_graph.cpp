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
    initUpdateIndex(1000000);
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
    //this->connectedComponents_->initUpdateIndex(currentComponent);
}

/*
void DynamicGraph::estimateConnectedComponents(){
    uint32_t startVisitedSize=this->graph_->getOutgoingIndexSize();
    uint32_t endVisitedSize=this->graph_->getIncomingIndexSize();
    uint32_t* startVisited=(uint32_t*)calloc(startVisitedSize,sizeof(uint32_t));
    uint32_t* endVisited=(uint32_t*)calloc(endVisitedSize,sizeof(uint32_t));
    uint32_t startVisitedKey=0;
    uint32_t endVisitedKey=0;

    Queue fringe;
    NodeIndex *incoming = this->incomingIndex_;
    NodeIndex *outgoing = this->outgoingIndex_;
    Buffer *inBuffer = this->incomingBuffer_;
    Buffer *outBuffer = this->outgoingBuffer_;

    while(true){

    }
}
*/

bool DynamicGraph::postAddWithVersion(uint32_t from, uint32_t to, uint32_t version){
    bool added = this->addWithVersion(from, to, version);
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

/*
long DynamicGraph::query(uint32_t from, uint32_t to){
    int32_t comp1, comp2;
    comp1 = this->connectedComponents_->findNodeConnectedComponentID(from);
    comp2 = this->connectedComponents_->findNodeConnectedComponentID(to);

    if(this->connectedComponents_->areConnected(comp1, comp2)){
        return Graph::query(from, to);
    }
    // If they dont belong in the same CC, we know no connection exists
    return -1;
    }*/

bool DynamicGraph::expandLevelWithVersion(NodeIndex* index, Buffer* buffer, Queue* queue, uint32_t myVisitedKey,
                                          uint32_t* myVisited, uint32_t targetVisitedKey, uint32_t* targetVisited, uint32_t& currentNeighbors, uint32_t version){
    // Expands the current nodes in the queue(one level).
    // Returns true if the desired path is found,false if not.

    ResultCode resCode;
    ptr currentNodePtr;
    ListNode* currentListNode;
    uint32_t* nodeNeighbors;
    uint32_t nodeNeighborsNumber;
    uint32_t* nodeProperties;

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
                nodeProperties = currentListNode->getPropertiesPtr();
                //For every neighbors inside the current list node
                nodeNeighborsNumber = currentListNode->getNeighborCount();
                for(uint32_t i = 0; i < nodeNeighborsNumber; i++){

                    // If our version is less than the egde's version
                    // ignore the edge
                    if(version < nodeProperties[i]){
                        //std::cout << "Versions: " << version << " " << nodeProperties[i] << std::endl;
                        continue;
                    }
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

long DynamicGraph::threadSafeQuery(uint32_t from, uint32_t to, uint32_t startVisitedKey,uint32_t* startVisited, uint32_t endVisitedKey, uint32_t* endVisited, uint32_t version){
    // Finds and returns the path distance from the source node to the target node.
    // Returns -1 if a paths does not exist.
    int32_t ccfrom, ccto;
    ccfrom = this->connectedComponents_->findNodeConnectedComponentID(from);
    ccto = this->connectedComponents_->findNodeConnectedComponentID(to);
    if(!this->connectedComponents_->areConnected(ccfrom, ccto)){
        return -1; // There is no way a path can exist between these
    }
    //if(from == 179098){
    //    std::cout << ccfrom << "-" << ccto << std::endl;
    //    std::cout << "==" << this->connectedComponents_->findNodeConnectedComponentID(1020) << "\n";
    //    std::cout << this->connectedComponents_->findNodeConnectedComponentID(1359969);
    //    std::cout << "----" << this->connectedComponents_->findNodeConnectedComponentID(1359970) << "\n";
    //}
    //std::cout << "Querying" << std::endl;
    if(from==to)
        return 0;

    // Validate nodes
    if(outgoingIndex_->getListHead(from) == PTR_NULL ||
       incomingIndex_->getListHead(to) == PTR_NULL){
        return -1;
    }

    /*
    //Create visited arrays if not created
    if(startVisited==NULL){
        startVisitedSize=outgoingIndex_->getCurrentSize();
        startVisited=(uint32_t*)calloc(startVisitedSize,sizeof(uint32_t));
    }
    //else if(startVisitedSize<outgoingIndex_->getCurrentSize())
        //cout << "startVistedSize Increased" << endl;
    if(endVisited==NULL){
        endVisitedSize=incomingIndex_->getCurrentSize();
        endVisited=(uint32_t*)calloc(endVisitedSize,sizeof(uint32_t));
    }
    */
    //else if(endVisitedSize<incomingIndex_->getCurrentSize())
        //cout << "endVistedSize Increased" << endl;

    //Check if maximum key has been reached and reset the visited arrays
    if(startVisitedKey==UINT_MAX){
        memset(startVisited,0,outgoingIndex_->getCurrentSize());
        startVisitedKey=0;
    }
    if(endVisitedKey==UINT_MAX){
        memset(endVisited,0,incomingIndex_->getCurrentSize());
        endVisitedKey=0;
    }
    //Increase the visited key of the current search
    startVisitedKey++;
    endVisitedKey++;

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
            if(expandLevelWithVersion(outgoingIndex_,outgoingBuffer_,&startQueue,startVisitedKey,
                           startVisited,endVisitedKey,endVisited,startCurrentNeighbors, version)==true)
                return startCurrentLength+endCurrentLength;
            startCurrentLength++;
            if(startQueue.isEmpty())
                return -1;
            startQueue.enqueue(LEVEL_END);
        }
        else{
            endCurrentNeighbors = 0;
            if(expandLevelWithVersion(incomingIndex_,incomingBuffer_,&endQueue,endVisitedKey,
                           endVisited,startVisitedKey,startVisited,endCurrentNeighbors, version)==true)
                return startCurrentLength+endCurrentLength;
            endCurrentLength++;
            if(endQueue.isEmpty())
                return -1;
            endQueue.enqueue(LEVEL_END);
        }

    }

    return -1;
}

/****************************/
/*---END OF DYNAMIC GRAPH---*/
/****************************/
