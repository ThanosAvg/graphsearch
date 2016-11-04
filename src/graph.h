#ifndef GRAPH_H
#define GRAPH_H

#include "node_index.h"

class Graph{
public:
    Graph();
    bool add(uint32_t from, uint32_t to);
    long query(uint32_t from, uint32_t to);
    ~Graph();
private:
    NodeIndex* incomingIndex_;
    NodeIndex* outgoingIndex_;

    Buffer* incomingBuffer_;
    Buffer* outgoingBuffer_;

    bool addToPair(NodeIndex* index, Buffer* buffer, uint32_t target, uint32_t node);
};
#endif
