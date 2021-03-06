#include "types.h"
#include "list_node.h"

ListNode::ListNode(){
    this->nextListNode_ = PTR_NULL;
    this->neighborCount_ = 0;
}

// bool ListNode::addNeighbor(uint32_t id){
//     if(this->neighborCount_ >= this->neighborMax_){
//         // We cannot add anymore
//         return false;
//     }
//     this->neighbors_[this->neighborCount_] = id;
//     this->neighborCount_++;
//     return true;
// }

bool ListNode::addNeighborWithProperty(uint32_t id, uint32_t property){
    if(this->neighborCount_ >= this->neighborMax_){
        // We cannot add anymore
        return false;
    }
    this->neighbors_[this->neighborCount_] = id;
    this->edgeProperty_[this->neighborCount_] = property;
    this->neighborCount_++;
    return true;
}

bool ListNode::addEdgeProperty(uint32_t x){
    // Not to be implemented at this point
    return false;
}

uint32_t* ListNode::getNeighborsPtr(){
    return neighbors_;
}

uint32_t* ListNode::getPropertiesPtr(){
    return edgeProperty_;
}

uint32_t ListNode::getNeighborCount(){
    return this->neighborCount_;
}

uint32_t ListNode::getNeighborMax(){
    return this->neighborMax_;
}

ptr ListNode::getNextListNode(){
    return this->nextListNode_;
}

void ListNode::setNextListNode(ptr next){
    this->nextListNode_ = next;
}

bool ListNode::containsNeighbor(uint32_t id){
    for(uint32_t i = 0; i < this->neighborCount_; i++){
        if(this->neighbors_[i] == id){
            return true;
        }
    }
    return false;
}
