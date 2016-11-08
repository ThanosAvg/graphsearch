#include <stdlib.h>
#include "queue.h"

Queue::Queue(){
    /*  Queue Constructor */
    start_=NULL;
    end_=NULL;
}

void Queue::enqueue(uint32_t node){
    /* Add a node in the end of the queue */
    struct QueueNode* temp = new QueueNode;
    temp->node = node;
    temp->next = NULL;
    if (start_ == NULL) {
        start_ = temp;
    }
    else {
        end_->next = temp;
    }
    end_ = temp;
}

uint32_t Queue::dequeue(){
    /* Pop the first node */
    uint32_t targetNode;
    struct QueueNode* temp;
    if(start_ != NULL){
        temp = start_;
        targetNode = temp->node;
        start_ = temp->next;
        if(start_ == NULL)
            end_ = NULL;
        delete temp;
    }
    return targetNode;
}

bool Queue::isEmpty(){
    /* Check if empty */
    return start_ == NULL;
}

Queue::~Queue(){
    /* Queue destructor */
    struct QueueNode* temp;
    while(!this->isEmpty()){
        temp = start_;
        start_ = start_->next;
        delete temp;
    }
}
