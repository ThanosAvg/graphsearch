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

    bool addWithVersion(uint32_t from, uint32_t to, uint32_t version);

    uint32_t getOutgoingIndexSize();
    uint32_t getIncomingIndexSize();

    // Takes 2 node ids and calculates the cost of the path between them, or -1 if either no
    // path exists or invalid input is given

    virtual long threadSafeQuery(uint32_t from, uint32_t to,uint32_t startVisitedKey,uint32_t* startVisited, uint32_t endVisitedKey, uint32_t* endVisited, uint32_t version) = 0;

    bool expandLevel(NodeIndex* index, Buffer* buffer, Queue* queue, uint32_t myVisitedKey,
        uint32_t* myVisited, uint32_t targetVisitedKey, uint32_t* targetVisited, uint32_t& currentNeighbors);
protected:
    NodeIndex* incomingIndex_;
    NodeIndex* outgoingIndex_;

    Buffer* incomingBuffer_;
    Buffer* outgoingBuffer_;

    uint32_t* startVisited;
    uint32_t startVisitedKey;
    uint32_t startVisitedSize;

    uint32_t* endVisited;
    uint32_t endVisitedKey;
    uint32_t endVisitedSize;

    // Private function to assist insertion in graph.
    // Takes either incomingIndex or outgoingIndex and their respective buffer
    // and performs insertion of neighbor node in target
    // checking for duplicates if checkDuplicates is true
    bool addToPair(NodeIndex* index, Buffer* buffer, uint32_t target, uint32_t node, uint32_t version);
    bool addToPairWithDupCheck(NodeIndex* index, Buffer* buffer, uint32_t target, uint32_t node, uint32_t version);
};

class StaticGraph: public Graph{
public:
    StaticGraph();
    ~StaticGraph();

    //SCC related functions
    void strongConnect(uint32_t node,uint32_t &index,Stack* stack,bool* onStack);
    bool estimateStronglyConnectedComponents();
    void estimateComponentsNeighbors(char select);

    bool expandLevelinComponent(NodeIndex* index, Buffer* buffer, Queue* queue, uint32_t myVisitedKey,
        uint32_t* myVisited, uint32_t targetVisitedKey, uint32_t* targetVisited, uint32_t& currentNeighbors, uint32_t componentId);
    bool expandLevelPrunned(NodeIndex* index, Buffer* buffer, Queue* queue, uint32_t myVisitedKey,
        uint32_t* myVisited, uint32_t targetVisitedKey, uint32_t* targetVisited, uint32_t& currentNeighbors,GrailIndex* grail,uint32_t target_node);

    //Threads
    long threadSafeQuery(uint32_t from, uint32_t to,uint32_t startVisitedKey,uint32_t* startVisited, uint32_t endVisitedKey, uint32_t* endVisited, uint32_t version);
    long threadEstimateShortestPathStronglyConnectedComponents(uint32_t source_node, uint32_t target_node,
        uint32_t startVisitedKey,uint32_t* startVisited, uint32_t endVisitedKey, uint32_t* endVisited);
    long threadEstimateShortestPathPrunned(uint32_t source_node, uint32_t target_node,
            uint32_t startVisitedKey,uint32_t* startVisited, uint32_t endVisitedKey, uint32_t* endVisited);

    //GrailRelatedFunctions
    uint32_t grailConnect(Component* currentComponent, GrailIndex* grail, int label, uint32_t &index,
        bool* visited, char select);
    void buildGrailIndex();
    GrailAnswer isReachableGrailIndex(uint32_t source_node, uint32_t target_node, GrailIndex* grail);

private:
    SCC* scc_;
    GrailIndex* grailIndexOutgoing_;
    GrailIndex* grailIndexIncoming_;
};
#endif
