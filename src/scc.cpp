#include "scc.h"
#include <cstdlib>
#include <iostream>

SCC::SCC(uint32_t numberOfVertices){
    //SCC constructor

    this->components_=(Component**)malloc(this->initialCmatrixLength_*sizeof(Component*));
    this->componentMatrixLength_=this->initialCmatrixLength_;
    this->componentsCount_=0;
    /* An array with number equal to the number of graph vertices initialized
    to zero */
    this->idBelongsToComponent_=(uint32_t*)calloc(numberOfVertices,sizeof(uint32_t));
}

SCC::~SCC(){
    //SCC destructor

    free(this->idBelongsToComponent_);
    Component* currentComponent;
    for(uint32_t i=0;i<this->componentsCount_;i++){
        currentComponent=this->components_[i];
        delete currentComponent->included_node_ids;
        free(currentComponent->componentNeighborsOutgoing);
        delete currentComponent;
    }
    free(this->components_);
}

uint32_t SCC::findNodeStronglyConnectedComponentID(uint32_t nodeId){
    return this->idBelongsToComponent_[nodeId];
}

void SCC::setComponent(Component* component){
    if(this->componentsCount_==this->componentMatrixLength_){
        this->components_=(Component**)realloc(this->components_,
        this->componentMatrixLength_*sizeof(Component*)*2);
        this->componentMatrixLength_*=2;
    }
    this->components_[this->componentsCount_]=component;
    this->componentsCount_++;
}

Component* SCC::getComponent(uint32_t component){
    return this->components_[component];
}

void SCC::setStronglyConnectedComponentID(uint32_t nodeId, uint32_t componentNum){
    this->idBelongsToComponent_[nodeId]=componentNum;
}

uint32_t SCC::getComponentsCount(){
    return this->componentsCount_;
}
