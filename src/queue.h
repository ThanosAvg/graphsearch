#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

struct QueueNode{
    uint32_t node;
    struct QueueNode* next;
};

class Queue{
public:
    Queue();
    void enqueue(uint32_t node);
    uint32_t dequeue();
    bool isEmpty();
    ~Queue();
private:
    QueueNode* start_;
    QueueNode* end_;
};

#endif
