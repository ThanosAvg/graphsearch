#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

class Queue{
public:
    Queue();
    void enqueue(uint32_t node);
    uint32_t dequeue();
    bool isEmpty();
    ~Queue();
private:
    static const uint32_t initialSize_ = 64;

    uint32_t* array_;
    uint32_t currentSize_;
    uint32_t start_;
    uint32_t end_;
};

#endif
