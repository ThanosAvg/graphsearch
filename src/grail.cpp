#include "grail.h"
#include <cstdlib>
#include <iostream>

using namespace std;

GrailIndex::GrailIndex(uint32_t indexSize){
    /* Create grail Index */

    this->index_=new grailNode*[indexSize];
    this->size_=indexSize;
    for(int i=0;i<indexSize;i++)
        this->index_[i]=new grailNode[this->labels];
}

uint32_t GrailIndex::getMinRank(int label,uint32_t indexPosition){
    /* Getter of min rank */

    grailNode* labelIndex=this->index_[indexPosition];
    return labelIndex[label].minRank;
}

uint32_t GrailIndex::getRank(int label,uint32_t indexPosition){
    /* Getter of rank */

    grailNode* labelIndex=this->index_[indexPosition];
    return labelIndex[label].rank;
}

uint32_t GrailIndex::setMinRank(int label,uint32_t indexPosition, uint32_t minRank){
    /* Setter of min rank */

    grailNode* labelIndex=this->index_[indexPosition];
    labelIndex[label].minRank=minRank;
}

uint32_t GrailIndex::setRank(int label,uint32_t indexPosition, uint32_t rank){
    /* Setter of rank */

    grailNode* labelIndex=this->index_[indexPosition];
    labelIndex[label].rank=rank;
}

bool GrailIndex::isSubset(uint32_t source, uint32_t target){
    /* Check if target node's set is subset of the source's set */

    for(int i=0;i<this->labels;i++){
        if(
            this->index_[target-1][i].minRank < this->index_[source-1][i].minRank ||
            this->index_[source-1][i].rank < this->index_[target-1][i].rank
        )
         return false;
    }
    return true;
}

GrailIndex::~GrailIndex(){
    /* Destroy Grail Index */

    for(int i=0;i<this->size_;i++)
        delete this->index_[i];
    delete this->index_;
}
