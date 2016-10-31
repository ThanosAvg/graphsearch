#ifndef GRAPH_H
#define GRAPH_H

#include "node_index.h"


class Graph{
public:
    Graph();
    bool add(uint32_t from, uint32_t to);
private:
    NodeIndex* incomingIndex_;
    NodeIndex* outgoingIndex_;

    Buffer* incomingBuffer_;
    Buffer* outgoingBuffer_;

    bool addToPair(NodeIndex* index, Buffer* buffer, uint32_t id);
};
#endif
