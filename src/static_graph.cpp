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

using namespace std;

StaticGraph::StaticGraph():Graph(){
    this->scc_=NULL;
    this->grailIndexOutgoing_=NULL;
    this->grailIndexIncoming_=NULL;
}

uint32_t StaticGraph::strongConnect(uint32_t node,uint32_t &index,Stack* stack,bool* onStack){

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

long StaticGraph::estimateShortestPathStronglyConnectedComponents(uint32_t source_node, uint32_t target_node){

    uint32_t componentId=this->scc_->findNodeStronglyConnectedComponentID(source_node);
    Component* currentComponent=this->scc_->getComponent(componentId-1);

    // Validate nodes
    if(this->outgoingIndex_->getListHead(source_node) == PTR_NULL ||
       this->incomingIndex_->getListHead(target_node) == PTR_NULL){
        return -1;
    }

    Queue startQueue, endQueue;

    Hash<uint32_t> startVisited((this)->closedSetSize_);
    Hash<uint32_t> endVisited((this)->closedSetSize_);
    ResultCode resCodeStart, resCodeEnd;

    startQueue.enqueue(UINT_MAX-1);
    endQueue.enqueue(UINT_MAX-1);

    uint32_t startCurrentNode,endCurrentNode;
    long startCurrentLength,endCurrentLength;
    ListNode* startCurrentListNode;
    ListNode* endCurrentListNode;
    ptr startCurrentNodePtr,endCurrentNodePtr;
    uint32_t* startNodeNeighbors;
    uint32_t* endNodeNeighbors;
    uint32_t startNodeNeighborsNumber;
    uint32_t endNodeNeighborsNumber;
    uint32_t startCurrentNeighbors=0;
    uint32_t endCurrentNeighbors=0;

    NodeIndex* outgoingIndex = this->outgoingIndex_;
    NodeIndex* incomingIndex = this->incomingIndex_;

    startCurrentLength = 0;
    endCurrentLength = 0;
    startCurrentNode = source_node;
    endCurrentNode = target_node;

    while(!startQueue.isEmpty() && !endQueue.isEmpty()){

    	if(startCurrentNeighbors <= endCurrentNeighbors){
            startCurrentNeighbors = 0;

            while(startCurrentNode != UINT_MAX-1){

                //If current Node is already visited skip
                startVisited.get(startCurrentNode, resCodeStart);
                if(resCodeStart == NOT_FOUND){
                    startVisited.add(startCurrentNode, startCurrentNode);
                    startCurrentNodePtr=outgoingIndex->getListHead(startCurrentNode);
                    if(startCurrentNodePtr == PTR_NULL){
                        continue;
                    }
                    startCurrentListNode=outgoingBuffer_->getListNode(startCurrentNodePtr);

                    //Push node's neighbors that are not in closed set
                    while(true){
                        startNodeNeighbors=startCurrentListNode->getNeighborsPtr();
                        //For every neighbors inside the current list node
                        startNodeNeighborsNumber=startCurrentListNode->getNeighborCount();
                        for(uint32_t i = 0; i < startNodeNeighborsNumber; i++){
                            if(this->scc_->findNodeStronglyConnectedComponentID(startNodeNeighbors[i])!=componentId)
                                continue;
                            //If current neighbor is the target one:return
                            endVisited.get(startNodeNeighbors[i], resCodeEnd);
                            if(resCodeEnd == FOUND){
                                return startCurrentLength+endCurrentLength;
                            }
                            startQueue.enqueue(startNodeNeighbors[i]);
                            startCurrentNeighbors++;
                        }
                        //Get the next list node pointer from the current one
                        startCurrentNodePtr=startCurrentListNode->getNextListNode();
                        if(startCurrentNodePtr == PTR_NULL)
                            break;
                        else
                            startCurrentListNode=outgoingBuffer_->getListNode(startCurrentNodePtr);
                    }
                }
                startCurrentNode = startQueue.dequeue();
            }

            //Update for next level
            startCurrentLength++;
            if(startQueue.isEmpty())
                break;
            startQueue.enqueue(UINT_MAX-1);
            startCurrentNode=startQueue.dequeue();
    	}

        //End side implementation

    	else{
            endCurrentNeighbors = 0;

            while(endCurrentNode != UINT_MAX-1){

                //If current Node is already visited skip
                endVisited.get(endCurrentNode, resCodeEnd);
                if(resCodeEnd == NOT_FOUND){
                    endVisited.add(endCurrentNode, endCurrentNode);
                    endCurrentNodePtr=incomingIndex->getListHead(endCurrentNode);
                    if(endCurrentNodePtr == PTR_NULL){
                        continue;
                    }
                    endCurrentListNode=incomingBuffer_->getListNode(endCurrentNodePtr);

                    //Push node's neighbors that are not in closed set
                    while(true){
                        endNodeNeighbors = endCurrentListNode->getNeighborsPtr();
                        //For every neighbors inside the current list node
                        endNodeNeighborsNumber=endCurrentListNode->getNeighborCount();
                        for(uint32_t i = 0; i < endNodeNeighborsNumber; i++){
                            if(this->scc_->findNodeStronglyConnectedComponentID(endNodeNeighbors[i])!=componentId)
                                continue;
                            //If current neighbor is the target one:return
                            startVisited.get(endNodeNeighbors[i], resCodeStart);
                            if(resCodeStart == FOUND){
                                return startCurrentLength + endCurrentLength;
                            }
                            endQueue.enqueue(endNodeNeighbors[i]);
                            endCurrentNeighbors++;
                        }
                        //Get the next list node pointer from the current one
                        endCurrentNodePtr = endCurrentListNode->getNextListNode();
                        if(endCurrentNodePtr == PTR_NULL)
                            break;
                        else
                            endCurrentListNode = incomingBuffer_->getListNode(endCurrentNodePtr);
                    }
                }
                endCurrentNode = endQueue.dequeue();
            }

            //Update for next level
            endCurrentLength++;
            if(endQueue.isEmpty())
                break;
            endQueue.enqueue(UINT_MAX-1);
            endCurrentNode = endQueue.dequeue();
        }
    }
    return -1;
}

uint32_t StaticGraph::grailConnect(Component* currentComponent,GrailIndex* grail,int label,uint32_t &index,bool* visited,char select){
    visited[currentComponent->component_id-1]=true;

    uint32_t componentNeighborsNumber;
    if(select=='o')
        componentNeighborsNumber=currentComponent->neighborNumberOutgoing;
    else
        componentNeighborsNumber=currentComponent->neighborNumberIncoming;

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
    }
    free(visited);
}

GrailAnswer StaticGraph::isReachableGrailIndex(uint32_t source_node,uint32_t target_node){

    GrailAnswer answer;
    uint32_t sourceComponentId=this->scc_->findNodeStronglyConnectedComponentID(source_node);
    uint32_t targetComponentId=this->scc_->findNodeStronglyConnectedComponentID(target_node);

    if(sourceComponentId==targetComponentId)
        answer=YES;
    else if(!this->grailIndexOutgoing_->isSubset(sourceComponentId,targetComponentId) ||
            !this->grailIndexIncoming_->isSubset(targetComponentId,sourceComponentId))
        answer=NO;
    else
        answer=MAYBE;

    return answer;
}

long StaticGraph::staticQuery(uint32_t from, uint32_t to){

    if(this->outgoingIndex_->getListHead(from) == PTR_NULL ||
       this->incomingIndex_->getListHead(to) == PTR_NULL){
        return -1;
    }

    GrailAnswer answer;

    answer=this->isReachableGrailIndex(from,to);

    if(answer==YES)
        return estimateShortestPathStronglyConnectedComponents(from,to);
    else if(answer==MAYBE)
        return query(from,to);
    else
        return -1;
}

StaticGraph::~StaticGraph(){
    delete this->scc_;
    delete this->grailIndexOutgoing_;
    delete this->grailIndexIncoming_;
}
