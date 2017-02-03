#include "graph.h"
#include "stack.h"
#include "queue.h"
#include "scc.h"
#include "grail.h"
#include "types.h"
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <time.h>
#include <algorithm>    // std::shuffle
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

using namespace std;

StaticGraph::StaticGraph():Graph(){
    this->scc_=NULL;
    this->grailIndexOutgoing_=NULL;
    this->grailIndexIncoming_=NULL;
}

uint32_t StaticGraph::strongConnect(uint32_t node, uint32_t &index, Stack* stack, bool* onStack){

    this->scc_->setStronglyConnectedComponentID(node,index);
    uint32_t vIndex=index;
    uint32_t vLowlink=index;
    uint32_t wLowLink;
    uint32_t wIndex;
    index++;
    stack->push(node);
    onStack[node]=true;

    ptr currentNodePtr=this->outgoingIndex_->getListHead(node);
    //If current Node has neighbors
    if(currentNodePtr != PTR_NULL){

        ListNode* currentListNode=this->outgoingBuffer_->getListNode(currentNodePtr);

        long neighborNumber=0;
        uint32_t* nodeNeighbors;

        while(true){
            nodeNeighbors=currentListNode->getNeighborsPtr();
            //For every neighbors inside the current list node
            neighborNumber=currentListNode->getNeighborCount();
            for(uint32_t i = 0; i < neighborNumber; i++){
                if(this->scc_->findNodeStronglyConnectedComponentID(nodeNeighbors[i])==0){
                    wLowLink=strongConnect(nodeNeighbors[i],index,stack,onStack);
                    vLowlink=((vLowlink < wLowLink) ? vLowlink : wLowLink);
                }
                else{
                    wIndex=this->scc_->findNodeStronglyConnectedComponentID(nodeNeighbors[i]);
                    if(onStack[nodeNeighbors[i]])
                        vLowlink=((vLowlink < wIndex) ? vLowlink : wIndex);
                }
            }
            //Get the next list node pointer from the current one
            currentNodePtr=currentListNode->getNextListNode();
            if(currentNodePtr == PTR_NULL)
                break;
            else
                currentListNode=this->outgoingBuffer_->getListNode(currentNodePtr);
        }
    }
    if(vLowlink == vIndex){
        Component* component=new Component;
        uint32_t componentId=this->scc_->getComponentsCount()+1;
        component->component_id=componentId;
        uint32_t sccNodesNumber=stack->count(node);
        component->included_nodes_count=sccNodesNumber;
        component->included_node_ids=new uint32_t[sccNodesNumber];
        component->componentNeighborsIncoming=(uint32_t*)malloc(10*sizeof(uint32_t));
        component->neighborNumberIncoming=0;
        component->neighborMatrixSizeIncoming=10;
        component->componentNeighborsOutgoing=(uint32_t*)malloc(10*sizeof(uint32_t));
        component->neighborNumberOutgoing=0;
        component->neighborMatrixSizeOutgoing=10;
        uint32_t newNode;

        for(uint32_t i=0;i<sccNodesNumber;i++){
            newNode= stack->pop();
            onStack[newNode]=false;
            component->included_node_ids[i]=newNode;
            this->scc_->setStronglyConnectedComponentID(newNode,componentId);
        }
        this->scc_->setComponent(component);
    }
    return vLowlink;
}

bool StaticGraph::estimateStronglyConnectedComponents(){
    uint32_t index=1;
    this->startVisitedSize=this->outgoingIndex_->getCurrentSize();
    this->endVisitedSize=this->incomingIndex_->getCurrentSize();

    // VISITED
    this->startVisited=(uint32_t*)calloc(this->startVisitedSize,sizeof(uint32_t));
    this->startVisitedKey=0;

    this->endVisited=(uint32_t*)calloc(this->endVisitedSize,sizeof(uint32_t));
    this->endVisitedKey=0;
    //VISITED
    uint32_t numberOfVertices=this->outgoingIndex_->getCurrentSize();
    this->scc_=new SCC(numberOfVertices);
    Stack* stack=new Stack();
    bool* onStack=(bool*)calloc(numberOfVertices,sizeof(bool));

    for(uint32_t node=0;node<numberOfVertices;node++){
        if(this->outgoingIndex_->getListHead(node)!=PTR_NULL &&
        this->scc_->findNodeStronglyConnectedComponentID(node)==0){
                strongConnect(node,index,stack,onStack);
        }
    }

    delete stack;
    free(onStack);
    return true;
}

void StaticGraph::estimateComponentsNeighbors(char select){

    NodeIndex* currentIndex;
    Buffer* currentBuffer;

    if(select=='i'){
        currentIndex=this->incomingIndex_;
        currentBuffer=this->incomingBuffer_;
    }
    else{
        currentIndex=this->outgoingIndex_;
        currentBuffer=this->outgoingBuffer_;
    }


    uint32_t componentsNumber=this->scc_->getComponentsCount();
    ptr currentNodePtr;
    uint32_t node;
    uint32_t nodesNumber;
    Component* currentComponent;
    ListNode* currentListNode;
    long neighborNodes;
    uint32_t* nodeNeighbors;
    uint32_t belongingComponent;
    bool isNeighbor;
    uint32_t sccNeighbors;

    for(uint32_t i=0;i<componentsNumber;i++){
        currentComponent=this->scc_->getComponent(i);
        nodesNumber=currentComponent->included_nodes_count;

        for(uint32_t currentNode=0;currentNode<nodesNumber;currentNode++){

            node=currentComponent->included_node_ids[currentNode];
            currentNodePtr=currentIndex->getListHead(node);
            if(currentNodePtr==PTR_NULL)
                continue;

            currentListNode=currentBuffer->getListNode(currentNodePtr);

            neighborNodes=0;

            while(true){
                nodeNeighbors=currentListNode->getNeighborsPtr();
                //For every neighbors inside the current list node
                neighborNodes=currentListNode->getNeighborCount();
                for(uint32_t j = 0; j < neighborNodes; j++){
                    belongingComponent=this->scc_->findNodeStronglyConnectedComponentID(nodeNeighbors[j]);
                    if(belongingComponent>0 && belongingComponent!=currentComponent->component_id){
                        if(select=='i'){
                            if(currentComponent->neighborNumberIncoming==currentComponent->neighborMatrixSizeIncoming){
                                currentComponent->componentNeighborsIncoming=(uint32_t*)realloc(currentComponent->componentNeighborsIncoming,
                                    currentComponent->neighborMatrixSizeIncoming*sizeof(uint32_t)*2);
                                currentComponent->neighborMatrixSizeIncoming*=2;
                            }
                            currentComponent->componentNeighborsIncoming[currentComponent->neighborNumberIncoming]=belongingComponent;
                            currentComponent->neighborNumberIncoming++;
                        }
                        else{
                            if(currentComponent->neighborNumberOutgoing==currentComponent->neighborMatrixSizeOutgoing){
                                currentComponent->componentNeighborsOutgoing=(uint32_t*)realloc(currentComponent->componentNeighborsOutgoing,
                                    currentComponent->neighborMatrixSizeOutgoing*sizeof(uint32_t)*2);
                                currentComponent->neighborMatrixSizeOutgoing*=2;
                            }
                            currentComponent->componentNeighborsOutgoing[currentComponent->neighborNumberOutgoing]=belongingComponent;
                            currentComponent->neighborNumberOutgoing++;
                        }
                    }
                }
                //Get the next list node pointer from the current one
                currentNodePtr=currentListNode->getNextListNode();
                if(currentNodePtr == PTR_NULL)
                    break;
                else
                    currentListNode=currentBuffer->getListNode(currentNodePtr);
            }
        }
    }
}

bool StaticGraph::expandLevelinComponent(NodeIndex* index, Buffer* buffer, Queue* queue, uint32_t myVisitedKey,
    uint32_t* myVisited, uint32_t targetVisitedKey, uint32_t* targetVisited, uint32_t& currentNeighbors, uint32_t componentId){
    /* Expands the current nodes in the queue(one level).Expands only neighbors that belong to the same component.
    Returns true if the desired path is found,false if not. */

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

            //Push node's neighbors that are not in closed set
            while(true){
                nodeNeighbors = currentListNode->getNeighborsPtr();
                //For every neighbors inside the current list node
                nodeNeighborsNumber = currentListNode->getNeighborCount();
                for(uint32_t i = 0; i < nodeNeighborsNumber; i++){
                    //Expand nodes belonging to the same component
                    if(this->scc_->findNodeStronglyConnectedComponentID(nodeNeighbors[i])==componentId){
                        //If current neighbor is the target one:return
                        if(targetVisited[nodeNeighbors[i]]==targetVisitedKey){
                            return true;
                        }
                        queue->enqueue(nodeNeighbors[i]);
                        currentNeighbors++;
                    }
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
    return false;
}

bool StaticGraph::expandLevelPrunned(NodeIndex* index, Buffer* buffer, Queue* queue, uint32_t myVisitedKey,
    uint32_t* myVisited, uint32_t targetVisitedKey, uint32_t* targetVisited, uint32_t& currentNeighbors,GrailIndex* grail, uint32_t target_node){
    /* Expands the current nodes in the queue(one level).
    Returns true if the desired path is found,false if not. */

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

            //Push node's neighbors that are not in closed set
            while(true){
                nodeNeighbors = currentListNode->getNeighborsPtr();
                //For every neighbors inside the current list node
                nodeNeighborsNumber = currentListNode->getNeighborCount();
                for(uint32_t i = 0; i < nodeNeighborsNumber; i++){
                    //Expand nodes belonging to the same component
                    if(isReachableGrailIndex(nodeNeighbors[i],target_node,grail)!=NO){
                        //If current neighbor is the target one:return
                        if(targetVisited[nodeNeighbors[i]]==targetVisitedKey){
                            return true;
                        }
                        queue->enqueue(nodeNeighbors[i]);
                        currentNeighbors++;
                    }
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
    return false;
}

long StaticGraph::estimateShortestPathStronglyConnectedComponents(uint32_t source_node, uint32_t target_node){
    /* Finds and returns the path distance from the source node to the target node.
    Returns -1 if a paths does not exist. */

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

    uint32_t componentId=this->scc_->findNodeStronglyConnectedComponentID(source_node);

    //Create two queues for each side for bi-bfs
    Queue startQueue, endQueue;

    //Push into the queues,the initial nodes for each side(level 0)
    startQueue.enqueue(source_node);
    endQueue.enqueue(target_node);
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
            if(expandLevelinComponent(this->outgoingIndex_,this->outgoingBuffer_,&startQueue,
                this->startVisitedKey,this->startVisited,this->endVisitedKey,this->endVisited,
                startCurrentNeighbors,componentId)==true)
                return startCurrentLength+endCurrentLength;
            startCurrentLength++;
            if(startQueue.isEmpty())
                return -1;
            startQueue.enqueue(LEVEL_END);
        }
        else{
            endCurrentNeighbors = 0;
            if(expandLevelinComponent(this->incomingIndex_,this->incomingBuffer_,&endQueue,
                this->endVisitedKey,this->endVisited,this->startVisitedKey,this->startVisited,
                endCurrentNeighbors,componentId)==true)
                return startCurrentLength+endCurrentLength;
            endCurrentLength++;
            if(endQueue.isEmpty())
                return -1;
            endQueue.enqueue(LEVEL_END);
        }

    }

    return -1;
}

long StaticGraph::estimateShortestPathPrunned(uint32_t source_node, uint32_t target_node){
    /* Finds and returns the path distance from the source node to the target node.
    Returns -1 if a paths does not exist. */

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
    startQueue.enqueue(source_node);
    endQueue.enqueue(target_node);
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
            if(expandLevelPrunned(this->outgoingIndex_,this->outgoingBuffer_,&startQueue,
                this->startVisitedKey,this->startVisited,this->endVisitedKey,this->endVisited,
                startCurrentNeighbors,this->grailIndexOutgoing_,target_node)==true)
                return startCurrentLength+endCurrentLength;
            startCurrentLength++;
            if(startQueue.isEmpty())
                return -1;
            startQueue.enqueue(LEVEL_END);
        }
        else{
            endCurrentNeighbors = 0;
            if(expandLevelPrunned(this->incomingIndex_,this->incomingBuffer_,&endQueue,
                this->endVisitedKey,this->endVisited,this->startVisitedKey,this->startVisited,
                endCurrentNeighbors,this->grailIndexIncoming_,source_node)==true)
                return startCurrentLength+endCurrentLength;
            endCurrentLength++;
            if(endQueue.isEmpty())
                return -1;
            endQueue.enqueue(LEVEL_END);
        }

    }

    return -1;
}

uint32_t StaticGraph::grailConnect(Component* currentComponent, GrailIndex* grail, int label,
     uint32_t &index, bool* visited, char select){
    visited[currentComponent->component_id-1]=true;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    uint32_t componentNeighborsNumber;
    if(select=='o'){
        componentNeighborsNumber=currentComponent->neighborNumberOutgoing;
        shuffle (&currentComponent->componentNeighborsOutgoing[0], &currentComponent->componentNeighborsOutgoing[componentNeighborsNumber], std::default_random_engine(seed));
    }
    else{
        componentNeighborsNumber=currentComponent->neighborNumberIncoming;
        shuffle (&currentComponent->componentNeighborsIncoming[0], &currentComponent->componentNeighborsIncoming[componentNeighborsNumber], std::default_random_engine(seed));
    }

    uint32_t childMinRank;
    uint32_t neighborComponentNode;
    uint32_t myMinRank=UINT_MAX;

    for(uint32_t i=0;i<componentNeighborsNumber;i++){
        if(select=='o')
            neighborComponentNode=currentComponent->componentNeighborsOutgoing[i]-1;
        else
            neighborComponentNode=currentComponent->componentNeighborsIncoming[i]-1;
        if(visited[neighborComponentNode]==false)
            grailConnect(this->scc_->getComponent(neighborComponentNode),grail,label,index,visited,select);
        childMinRank=grail->getMinRank(label,neighborComponentNode);
        myMinRank=((myMinRank < childMinRank) ? myMinRank : childMinRank);
    }
    grail->setRank(label,currentComponent->component_id-1,index);
    if(myMinRank==UINT_MAX)
        myMinRank=index;
    grail->setMinRank(label,currentComponent->component_id-1,myMinRank);
    index++;

}

void StaticGraph::buildGrailIndex(){
    uint32_t index=1;
    uint32_t indexSize=this->scc_->getComponentsCount();
    this->grailIndexOutgoing_=new GrailIndex(indexSize);
    Component* currentComponent;
    bool* visited=(bool*)calloc(indexSize,sizeof(bool));
    int labelNumber=this->grailIndexOutgoing_->labels;

    for(uint32_t label=0;label<labelNumber;label++){

        for(uint32_t i=0;i<indexSize;i++){
            currentComponent=this->scc_->getComponent(i);
            if(visited[i]==false){
                grailConnect(currentComponent,this->grailIndexOutgoing_,label,index,visited,'o');
            }
        }
        memset(visited,0,indexSize);
        index=1;
    }
    memset(visited,0,indexSize);

    this->grailIndexIncoming_=new GrailIndex(indexSize);

    for(uint32_t label=0;label<labelNumber;label++){

        for(uint32_t i=0;i<indexSize;i++){
            currentComponent=this->scc_->getComponent(i);
            if(visited[i]==false){
                grailConnect(currentComponent,this->grailIndexIncoming_,label,index,visited,'i');
            }
        }
        memset(visited,0,indexSize);
        index=1;
    }
    free(visited);
}

GrailAnswer StaticGraph::isReachableGrailIndex(uint32_t source_node, uint32_t target_node, GrailIndex* grail){

    GrailAnswer answer;
    uint32_t sourceComponentId=this->scc_->findNodeStronglyConnectedComponentID(source_node);
    uint32_t targetComponentId=this->scc_->findNodeStronglyConnectedComponentID(target_node);

    if(sourceComponentId==targetComponentId)
        answer=YES;
    else if(!grail->isSubset(sourceComponentId,targetComponentId))
        answer=NO;
    else
        answer=MAYBE;

    return answer;
}

long StaticGraph::staticQuery(uint32_t from, uint32_t to){

    if(from==to)
        return 0;

    if(this->outgoingIndex_->getListHead(from) == PTR_NULL ||
       this->incomingIndex_->getListHead(to) == PTR_NULL){
        return -1;
    }

    GrailAnswer answer;

    answer=this->isReachableGrailIndex(from,to,this->grailIndexOutgoing_);
    if(answer==YES)
        return estimateShortestPathStronglyConnectedComponents(from,to);
    else if(answer==NO)
        return -1;
    else{
        answer=this->isReachableGrailIndex(to,from,this->grailIndexIncoming_);
        if(answer==YES)
            return estimateShortestPathStronglyConnectedComponents(from,to);
        else if(answer==NO)
            return -1;
        else
            return estimateShortestPathPrunned(from,to);
    }
}

StaticGraph::~StaticGraph(){
    delete this->scc_;
    delete this->grailIndexOutgoing_;
    delete this->grailIndexIncoming_;
}

long StaticGraph::threadSafeQuery(uint32_t from, uint32_t to,
                                  uint32_t startVisitedKey,uint32_t* startVisited, uint32_t endVisitedKey, uint32_t* endVisited, uint32_t version){

    if(from==to)
        return 0;

    if(this->outgoingIndex_->getListHead(from) == PTR_NULL ||
       this->incomingIndex_->getListHead(to) == PTR_NULL){
        return -1;
    }

    GrailAnswer answer;

    answer=this->isReachableGrailIndex(from,to,this->grailIndexOutgoing_);
    if(answer==YES)
        return threadEstimateShortestPathStronglyConnectedComponents(from,to,startVisitedKey,startVisited,endVisitedKey,endVisited);
    else if(answer==NO)
        return -1;
    else{
        answer=this->isReachableGrailIndex(to,from,this->grailIndexIncoming_);
        if(answer==YES)
            return threadEstimateShortestPathStronglyConnectedComponents(from,to,startVisitedKey,startVisited,endVisitedKey,endVisited);
        else if(answer==NO)
            return -1;
        else
            return threadEstimateShortestPathPrunned(from,to,startVisitedKey,startVisited,endVisitedKey,endVisited);
    }
}


long StaticGraph::threadEstimateShortestPathStronglyConnectedComponents(uint32_t source_node, uint32_t target_node,
    uint32_t startVisitedKey,uint32_t* startVisited, uint32_t endVisitedKey, uint32_t* endVisited){
    /* Finds and returns the path distance from the source node to the target node.
    Returns -1 if a paths does not exist. */

    uint32_t componentId=this->scc_->findNodeStronglyConnectedComponentID(source_node);

    //Create two queues for each side for bi-bfs
    Queue startQueue, endQueue;

    //Push into the queues,the initial nodes for each side(level 0)
    startQueue.enqueue(source_node);
    endQueue.enqueue(target_node);
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
            if(expandLevelinComponent(this->outgoingIndex_,this->outgoingBuffer_,&startQueue,
                startVisitedKey,startVisited,endVisitedKey,endVisited,
                startCurrentNeighbors,componentId)==true)
                return startCurrentLength+endCurrentLength;
            startCurrentLength++;
            if(startQueue.isEmpty())
                return -1;
            startQueue.enqueue(LEVEL_END);
        }
        else{
            endCurrentNeighbors = 0;
            if(expandLevelinComponent(this->incomingIndex_,this->incomingBuffer_,&endQueue,
                endVisitedKey,endVisited,startVisitedKey,startVisited,
                endCurrentNeighbors,componentId)==true)
                return startCurrentLength+endCurrentLength;
            endCurrentLength++;
            if(endQueue.isEmpty())
                return -1;
            endQueue.enqueue(LEVEL_END);
        }

    }

    return -1;
}

long StaticGraph::threadEstimateShortestPathPrunned(uint32_t source_node, uint32_t target_node,
    uint32_t startVisitedKey,uint32_t* startVisited, uint32_t endVisitedKey, uint32_t* endVisited){
    /* Finds and returns the path distance from the source node to the target node.
    Returns -1 if a paths does not exist. */

    //Create two queues for each side for bi-bfs
    Queue startQueue, endQueue;

    //Push into the queues,the initial nodes for each side(level 0)
    startQueue.enqueue(source_node);
    endQueue.enqueue(target_node);
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
            if(expandLevelPrunned(this->outgoingIndex_,this->outgoingBuffer_,&startQueue,
                startVisitedKey,startVisited,endVisitedKey,endVisited,
                startCurrentNeighbors,this->grailIndexOutgoing_,target_node)==true)
                return startCurrentLength+endCurrentLength;
            startCurrentLength++;
            if(startQueue.isEmpty())
                return -1;
            startQueue.enqueue(LEVEL_END);
        }
        else{
            endCurrentNeighbors = 0;
            if(expandLevelPrunned(this->incomingIndex_,this->incomingBuffer_,&endQueue,
                endVisitedKey,endVisited,startVisitedKey,startVisited,
                endCurrentNeighbors,this->grailIndexIncoming_,source_node)==true)
                return startCurrentLength+endCurrentLength;
            endCurrentLength++;
            if(endQueue.isEmpty())
                return -1;
            endQueue.enqueue(LEVEL_END);
        }

    }

    return -1;
}

