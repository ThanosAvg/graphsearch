#include <stdlib.h>
#include <limits.h>
#include "queue.h"

Queue::Queue(){
    /*  Queue Constructor */
    start_=0;
    end_=0;
    currentSize_=initialSize_;
    array_=(uint32_t*)malloc(initialSize_*sizeof(uint32_t));
}

void Queue::enqueue(uint32_t node){
    /* Add a node in the end of the queue */
    if ( end_ == currentSize_){
        array_=(uint32_t*)realloc(array_,currentSize_*sizeof(uint32_t)*2);
        currentSize_*=2;
    }
    array_[end_] = node;
    end_++;
}

uint32_t Queue::dequeue(){
    /* Pop the first node */
    uint32_t targetNode=array_[start_];
    start_++;
    return targetNode;
}

bool Queue::isEmpty(){
    /* Check if empty */
    return start_ == end_;
}

Queue::~Queue(){
    /* Queue destructor */
    free(array_);
}
