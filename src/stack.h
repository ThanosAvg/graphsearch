#ifndef STACK_H
#define STACK_H

#include "scc.h"
#include <stdint.h>

class Stack{
public:
    Stack();
    void push(uint32_t);
    uint32_t pop();
    uint32_t count(uint32_t);
    bool isEmpty();
    ~Stack();
private:
    static const uint32_t initialSize_ = 1024;

    uint32_t pointer_;
    uint32_t* array_;
    uint32_t currentSize_;
};

#endif
