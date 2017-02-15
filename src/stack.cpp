#include <stdlib.h>
#include <iostream>
#include "stack.h"

Stack::Stack(){
    //Stack Constructor
    pointer_=0;
    currentSize_=initialSize_;
    array_=(uint32_t*)malloc(initialSize_*sizeof(uint32_t));
}

Stack::~Stack(){
    //Stack destructor
    free(array_);
}

void Stack::push(uint32_t node){
    //Push a node in the stack
    if ( pointer_ == currentSize_ ){
        array_=(uint32_t*)realloc(array_,currentSize_*sizeof(uint32_t)*2);
        currentSize_*=2;
    }
    array_[pointer_] = node;
    pointer_++;
}

uint32_t Stack::pop(){
    //Pop the first node
    uint32_t targetNode=array_[pointer_-1];
    pointer_--;
    return targetNode;
}

uint32_t Stack::count(uint32_t targetNode){
    //Count nodes until the targetNode
    for(uint32_t i=pointer_-1;i>=0;i--){
        if(array_[i]==targetNode){
            return pointer_-i;
        }
    }
}

bool Stack::isEmpty(){
    //Check if empty
    return pointer_ == 0;
}
