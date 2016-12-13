#ifndef GRAPH_H
#define GRAPH_H

#include "node_index.h"
#include "stack.h"
#include "queue.h"
#include "scc.h"
#include "grail.h"

class Graph{
public:
    Graph();
    ~Graph();

    // Takes 2 node ids and inserts them to the graph.
    // Returns false if insertion fails, true otherwise
    bool add(uint32_t from, uint32_t to);

    // Takes 2 node ids and calculates the cost of the path between them, or -1 if either no
    // path exists or invalid input is given
    long query(uint32_t from, uint32_t to);
    bool expandLevel(NodeIndex* index,Buffer* buffer,Queue* queue,Hash<uint32_t>* myVisited,Hash<uint32_t>* otherVisited,uint32_t& currentNeighbors);
protected:
    static const uint32_t closedSetSize_ = 37; //131;

    NodeIndex* incomingIndex_;
    NodeIndex* outgoingIndex_;

    Buffer* incomingBuffer_;
    Buffer* outgoingBuffer_;

    Hash<uint32_t> startVisited;
    Hash<uint32_t> endVisited;

    // Private function to assist insertion in graph.
    // Takes either incomingIndex or outgoingIndex and their respective buffer
    // and performs insertion of neighbor node in target
    // checking for duplicates if checkDuplicates is true
    bool addToPair(NodeIndex* index, Buffer* buffer, uint32_t target, uint32_t node);
    bool addToPairWithDupCheck(NodeIndex* index, Buffer* buffer, uint32_t target, uint32_t node);
};

class StaticGraph: public Graph{
public:
    StaticGraph();
    ~StaticGraph();

    //SCC related functions
    uint32_t strongConnect(uint32_t, uint32_t &, Stack*, bool*);
    bool estimateStronglyConnectedComponents();
    void estimateComponentsNeighbors(char select);
    long estimateShortestPathStronglyConnectedComponents(uint32_t, uint32_t);

    //GrailRelatedFunctions
    uint32_t grailConnect(Component*, GrailIndex*, int, uint32_t &, bool*, char);
    void buildGrailIndex();
    GrailAnswer isReachableGrailIndex(uint32_t, uint32_t);

    long staticQuery(uint32_t,uint32_t);

private:
    SCC* scc_;
    GrailIndex* grailIndexOutgoing_;
    GrailIndex* grailIndexIncoming_;
};
#endif
