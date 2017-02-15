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
#include <algorithm>
#include <random>
#include <chrono>

StaticGraph::StaticGraph():Graph(){
    //Static graph constructor
    this->scc_=NULL;
    this->grailIndexOutgoing_=NULL;
    this->grailIndexIncoming_=NULL;
}

StaticGraph::~StaticGraph(){
    //Static graph destructor
    delete this->scc_;
    delete this->grailIndexOutgoing_;
    delete this->grailIndexIncoming_;
}

void StaticGraph::strongConnect(uint32_t node, uint32_t &index, Stack* stack, bool* onStack){
    //Iterative version of tarjan's algorithm for calculation of strongly connencted components

    //Use v node for current node
    this->scc_->setStronglyConnectedComponentID(node,index);
    SccNode* v=new SccNode;
    v->node=node;
    v->index=index;
    v->lowlink=index;
    index++;
    stack->push(node);
    onStack[node]=true;
    v->currentNodePtr=this->outgoingIndex_->getListHead(node);
    if(v->currentNodePtr != PTR_NULL){
        v->currentListNode=this->outgoingBuffer_->getListNode(v->currentNodePtr);
        v->nodeNeighbors=v->currentListNode->getNeighborsPtr();
        v->neighborNumber=v->currentListNode->getNeighborCount();
        v->nextNeighbor=0;
    }
    v->previous=NULL;
    uint32_t wIndex;

    while(true){
        //If current Node has neighbors
        if(v->currentNodePtr != PTR_NULL){

            if(this->scc_->findNodeStronglyConnectedComponentID(v->nodeNeighbors[v->nextNeighbor])==0){
                //Iterative version of tarjan's recursive call using a memory allocated stack
                this->scc_->setStronglyConnectedComponentID(v->nodeNeighbors[v->nextNeighbor],index);
                SccNode* w=new SccNode;
                w->node=v->nodeNeighbors[v->nextNeighbor];
                w->index=index;
                w->lowlink=index;
                index++;
                stack->push(w->node);
                onStack[w->node]=true;
                w->currentNodePtr=this->outgoingIndex_->getListHead(w->node);
                if(w->currentNodePtr != PTR_NULL){
                    w->currentListNode=this->outgoingBuffer_->getListNode(w->currentNodePtr);
                    w->nodeNeighbors=w->currentListNode->getNeighborsPtr();
                    w->neighborNumber=w->currentListNode->getNeighborCount();
                    w->nextNeighbor=-1;
                }
                w->previous=v;
                v=w;
            }
            else{
                wIndex=this->scc_->findNodeStronglyConnectedComponentID(v->nodeNeighbors[v->nextNeighbor]);
                if(onStack[v->nodeNeighbors[v->nextNeighbor]])
                    v->lowlink=((v->lowlink < wIndex) ? v->lowlink : wIndex);
            }

            v->nextNeighbor++;
            //Get the next list node pointer from the current one
            if(v->nextNeighbor==v->neighborNumber && v->currentNodePtr != PTR_NULL){
                v->currentNodePtr=v->currentListNode->getNextListNode();
                if(v->currentNodePtr != PTR_NULL){
                    v->currentListNode=this->outgoingBuffer_->getListNode(v->currentNodePtr);
                    v->nodeNeighbors=v->currentListNode->getNeighborsPtr();
                    v->neighborNumber=v->currentListNode->getNeighborCount();
                    v->nextNeighbor=0;
                }
            }
        }
        else{
            if(v->lowlink == v->index){
                //Create a new Component
                Component* component=new Component;
                uint32_t componentId=this->scc_->getComponentsCount()+1;
                component->component_id=componentId;
                uint32_t sccNodesNumber=stack->count(v->node);
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

            if(v->previous!=NULL){
                SccNode* prev=v->previous;
                prev->lowlink=((prev->lowlink < v->lowlink) ? prev->lowlink : v->lowlink);
                delete v;
                v=prev;
            }
            else{
                break;
            }
        }
    }
}

bool StaticGraph::estimateStronglyConnectedComponents(){
    //Functions that creates the scc's using the strongConnect function for
    //every node

    uint32_t index=1;
    this->startVisitedSize=this->outgoingIndex_->getCurrentSize();
    this->endVisitedSize=this->incomingIndex_->getCurrentSize();

    this->startVisited=(uint32_t*)calloc(this->startVisitedSize,sizeof(uint32_t));
    this->startVisitedKey=0;

    this->endVisited=(uint32_t*)calloc(this->endVisitedSize,sizeof(uint32_t));
    this->endVisitedKey=0;

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
    //Calculate the neighbors of each component using the indexes and list nodes
    //The select argument changes the calculation between the 2 index types

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
            if(currentNodePtr==PTR_NULL){
                continue;
            }

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
                if(currentNodePtr == PTR_NULL){
                    break;
                }
                else{
                    currentListNode=currentBuffer->getListNode(currentNodePtr);
                }
            }
        }
    }
}

uint32_t StaticGraph::grailConnect(Component* currentComponent, GrailIndex* grail, int label,
     uint32_t &index, bool* visited, char select){
         //Function for the calculation of the grail index
         //Uses randomization between the components neighbors for each label
    visited[currentComponent->component_id-1]=true;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    uint32_t componentNeighborsNumber;
    //Select between incoming and outgoing index
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

    //Calculation of the grail ranks
    for(uint32_t i=0;i<componentNeighborsNumber;i++){
        if(select=='o'){
            neighborComponentNode=currentComponent->componentNeighborsOutgoing[i]-1;
        }
        else{
            neighborComponentNode=currentComponent->componentNeighborsIncoming[i]-1;
        }
        if(visited[neighborComponentNode]==false){
            grailConnect(this->scc_->getComponent(neighborComponentNode),grail,label,index,visited,select);
        }
        childMinRank=grail->getMinRank(label,neighborComponentNode);
        myMinRank=((myMinRank < childMinRank) ? myMinRank : childMinRank);
    }
    grail->setRank(label,currentComponent->component_id-1,index);
    if(myMinRank==UINT_MAX){
        myMinRank=index;
    }
    grail->setMinRank(label,currentComponent->component_id-1,myMinRank);
    index++;

}

void StaticGraph::buildGrailIndex(){
    //Function for the construction of the grail index
    //Calls the grailConnect function label times for each component
    //Build 2 grail indexes for the incoming and outgoing nodes
    //Uses a boolean array for the visited nodes

    uint32_t index=1;
    uint32_t indexSize=this->scc_->getComponentsCount();
    this->grailIndexOutgoing_=new GrailIndex(indexSize);
    Component* currentComponent;
    bool* visited=(bool*)calloc(indexSize,sizeof(bool));
    int labelNumber=this->grailIndexOutgoing_->labels;

    //Outgoing grail index construction
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

    //Incoming grail index construction
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
    //Checks the relationship between two nodes using the grail index
    //Returns YES if they belong to the same component,NO if the source node cannot
    //reach the target node and MAYBE otherwise

    GrailAnswer answer;
    uint32_t sourceComponentId=this->scc_->findNodeStronglyConnectedComponentID(source_node);
    uint32_t targetComponentId=this->scc_->findNodeStronglyConnectedComponentID(target_node);

    if(sourceComponentId==targetComponentId){
        answer=YES;
    }
    else if(!grail->isSubset(sourceComponentId,targetComponentId)){
        answer=NO;
    }
    else{
        answer=MAYBE;
    }

    return answer;
}

long StaticGraph::threadSafeQuery(uint32_t from, uint32_t to,
                                  uint32_t startVisitedKey,uint32_t* startVisited, uint32_t endVisitedKey, uint32_t* endVisited, uint32_t version){

    if(from==to){
        return 0;
    }

    if(this->outgoingIndex_->getListHead(from) == PTR_NULL ||
       this->incomingIndex_->getListHead(to) == PTR_NULL){
        return -1;
    }

    GrailAnswer answer;

    answer=this->isReachableGrailIndex(from,to,this->grailIndexOutgoing_);
    if(answer==YES){
        return threadEstimateShortestPathStronglyConnectedComponents(from,to,startVisitedKey,startVisited,endVisitedKey,endVisited);
    }
    else if(answer==NO){
        return -1;
    }
    else{
        answer=this->isReachableGrailIndex(to,from,this->grailIndexIncoming_);
        if(answer==YES){
            return threadEstimateShortestPathStronglyConnectedComponents(from,to,startVisitedKey,startVisited,endVisitedKey,endVisited);
        }
        else if(answer==NO){
            return -1;
        }
        else{
            return threadEstimateShortestPathPrunned(from,to,startVisitedKey,startVisited,endVisitedKey,endVisited);
        }
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
            if(startQueue.isEmpty()){
                return -1;
            }
            startQueue.enqueue(LEVEL_END);
        }
        else{
            endCurrentNeighbors = 0;
            if(expandLevelinComponent(this->incomingIndex_,this->incomingBuffer_,&endQueue,
                endVisitedKey,endVisited,startVisitedKey,startVisited,
                endCurrentNeighbors,componentId)==true)
                return startCurrentLength+endCurrentLength;
            endCurrentLength++;
            if(endQueue.isEmpty()){
                return -1;
            }
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
            if(startQueue.isEmpty()){
                return -1;
            }
            startQueue.enqueue(LEVEL_END);
        }
        else{
            endCurrentNeighbors = 0;
            if(expandLevelPrunned(this->incomingIndex_,this->incomingBuffer_,&endQueue,
                endVisitedKey,endVisited,startVisitedKey,startVisited,
                endCurrentNeighbors,this->grailIndexIncoming_,source_node)==true)
                return startCurrentLength+endCurrentLength;
            endCurrentLength++;
            if(endQueue.isEmpty()){
                return -1;
            }
            endQueue.enqueue(LEVEL_END);
        }

    }

    return -1;
}

bool StaticGraph::expandLevelinComponent(NodeIndex* index, Buffer* buffer, Queue* queue, uint32_t myVisitedKey,
    uint32_t* myVisited, uint32_t targetVisitedKey, uint32_t* targetVisited, uint32_t& currentNeighbors, uint32_t componentId){
    // Expands the current nodes in the queue(one level).Expands only neighbors that belong to the same component.
    //Returns true if the desired path is found,false if not.

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
                if(currentNodePtr == PTR_NULL){
                    break;
                }
                else{
                    currentListNode = buffer->getListNode(currentNodePtr);
                }
            }
        }
        currentNode = queue->dequeue();
    }
    return false;
}

bool StaticGraph::expandLevelPrunned(NodeIndex* index, Buffer* buffer, Queue* queue, uint32_t myVisitedKey,
    uint32_t* myVisited, uint32_t targetVisitedKey, uint32_t* targetVisited, uint32_t& currentNeighbors,GrailIndex* grail, uint32_t target_node){
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
                if(currentNodePtr == PTR_NULL){
                    break;
                }
                else{
                    currentListNode = buffer->getListNode(currentNodePtr);
                }
            }
        }
        currentNode = queue->dequeue();
    }
    return false;
}
