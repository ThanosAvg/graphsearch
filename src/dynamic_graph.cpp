#include "dynamic_graph.h"
#include "queue.h"

/****************************/
/*----------CC CODE---------*/
/****************************/

CC::CC(){
    /* Constructs a CCs assisting data structs. */
    lastComponent = 0;
    updateIndexSize_ = 0;
    ccindex_ = (int32_t*) malloc(ccLimit_ * sizeof(int32_t));
    memset(ccindex_, -1, ccLimit_ * sizeof(int32_t));
    ccSize_ = 0;
    ccMax_ = ccLimit_;
    updateIndexSize_ = updateInitial_;
    updateIndex = (int32_t *) malloc(sizeof(int32_t) * updateIndexSize_);
    memset(updateIndex, -1, sizeof(int32_t) * updateIndexSize_);
    merged_ = (int32_t *) malloc(sizeof(int32_t) * mergedInitial_);
    mergedSize_ = 0;
    mergedMax_ = mergedInitial_;
    mergedHits = 0;
}

CC::~CC(){
    free(ccindex_);
    free(updateIndex);
    free(merged_);
}

void CC::addNodeToComponent(uint32_t nodeId, int32_t ccId){
    /* Adds a node to a CC. */
    if(nodeId > ccMax_){
        int32_t *resized;
        uint32_t newSize = 2 * ccMax_;
        if(nodeId >= newSize){
            newSize = nodeId + 1;
        }
        resized = (int32_t *) realloc(ccindex_, sizeof(int32_t) * newSize);
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
    /* Adds given node to given CC and reconfigures update index. */
    addNodeToComponent(nodeId, ccId);

    if(ccId > (int32_t) this->updateIndexSize_){
        int32_t *resized;
        uint32_t newSize = 2 * this->updateIndexSize_;
        if(ccId > (int32_t) newSize){
            newSize = ccId; // Double or use ccid; whichever bigger
        }
        resized = (int32_t*) realloc(this->updateIndex, newSize * sizeof(int32_t));
        if(resized == NULL){
            std::cout << "Error allocating!" << std::endl;
            return;
        }
        this->updateIndexSize_ = newSize;
        this->updateIndex = resized;
    }
}

void CC::joinComponents(int32_t comp1, int32_t comp2){
    /* Joins components using update index. */
    int32_t commonCC = updateIndex[comp1];
    if(commonCC == -1){
        updateIndex[comp1] = comp1;
        commonCC = comp1;
    }
    int32_t toBeMerged = updateIndex[comp2];


    bool duplicateComp1 = false;
    bool duplicateComp2 = false;

    updateIndex[comp2] = commonCC;
    if(toBeMerged != -1){
        for(uint32_t i = 0; i < mergedSize_; i++){
            if(updateIndex[merged_[i]] == toBeMerged){
                updateIndex[merged_[i]] = commonCC;
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
    /* Fetches the CC id of given node. */
    return ccindex_[nodeId];
}

bool CC::areConnected(int32_t comp1, int32_t comp2){
    /* Checks if 2 CCs are connected. */
    if(comp1 == comp2) {
        return true;
    }
    else if(comp1 == -1 || comp2 == -1){
        return false;
    }
    else if(updateIndex[comp1] == -1 && updateIndex[comp2] == -1){
        return false;
    }
    else if(updateIndex[comp1] == updateIndex[comp2]){
        return true;
    }
    else{
        return false;
    }
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

DynamicGraph::DynamicGraph() {
}

DynamicGraph::~DynamicGraph(){
}

bool DynamicGraph::postAddWithVersion(uint32_t from, uint32_t to, uint32_t version){
    /* Adds an edge by using the cc and the update index */
    bool added = this->addWithVersion(from, to, version);
    if(added){
        int32_t cfrom, cto;
        cfrom = this->connectedComponents_.findNodeConnectedComponentID(from);
        cto = this->connectedComponents_.findNodeConnectedComponentID(to);

        if(cfrom == -1 && cto == -1){ // Case both do not exist
            // Create them both and add them to the same CC
            this->connectedComponents_.lastComponent++;
            this->connectedComponents_.postAddNodeToComponent(from, this->connectedComponents_.lastComponent);
            cfrom = this->connectedComponents_.findNodeConnectedComponentID(from);
            this->connectedComponents_.postAddNodeToComponent(to, this->connectedComponents_.lastComponent);
            cto = this->connectedComponents_.findNodeConnectedComponentID(to);
        }
        else{
            if(cfrom == -1){ // Only from exists
                // Create it and put it on to CC
                this->connectedComponents_.postAddNodeToComponent(from, this->connectedComponents_.findNodeConnectedComponentID(to));
            }
            else if(cto == -1){ // Only to exists
                // Create it and put it on from CC
                this->connectedComponents_.postAddNodeToComponent(to, this->connectedComponents_.findNodeConnectedComponentID(from));
            }
            else{
                // Both CCs exist
                // Join them if not already connected
                if(!this->connectedComponents_.areConnected(cfrom, cto)){
                    this->connectedComponents_.joinComponents(cfrom, cto);
                }
            }
        }
        return true;
    }
    else{
        return false;
    }
}

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
    int32_t comp1, comp2;
    comp1 = this->connectedComponents_.findNodeConnectedComponentID(from);
    comp2 = this->connectedComponents_.findNodeConnectedComponentID(to);

    /* Check if a connection exists even in a future version.
     * False positives dont hurt. */

    if(comp1 == -1 || comp2 == -1){
        return -1;
    }
    else if(!(connectedComponents_.updateIndex[comp1] == connectedComponents_.updateIndex[comp2])){
        return -1;
    }


    if(from==to)
        return 0;

    // Validate nodes
    if(outgoingIndex_->getListHead(from) == PTR_NULL ||
       incomingIndex_->getListHead(to) == PTR_NULL){
        return -1;
    }

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
