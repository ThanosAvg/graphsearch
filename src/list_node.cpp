#include "types.h"
#include "list_node.h"

ListNode::ListNode(){
    this->nextListNode_ = PTR_NULL;
    this->neighborCount_ = 0;
}

bool ListNode::addNeighbor(uint32_t id){
    if(this->neighborCount_ >= this->neighborMax_){
        // We cannot add anymore
        return false;
    }
    this->neighbors_[this->neighborCount_] = id;
    this->neighborCount_++;
    return true;
}

bool ListNode::addEdgeProperty(uint32_t x){
    // Not to be implemented at this point
    return false;
}

uint32_t ListNode::getNeighborCount(){
    return this->neighborCount_;
}
