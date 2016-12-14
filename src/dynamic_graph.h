#ifndef DYNAMIC_GRAPH_H
#define DYNAMIC_GRAPH_H

#include <stdint.h>

#include "hashtable.h"
#include "graph.h"

struct ConnectData{
    int flag;
    uint32_t size;
    uint32_t max;
    uint32_t *connected;
};

class CC{
    public:
    CC();
    ~CC();
    void addNodeToComponent(uint32_t nodeId, int32_t ccId);
    void postAddNodeToComponent(uint32_t nodeId, int32_t ccId);
    //bool insertNewEdge(uint32_t nodeIdS, uint32_t nodeIdE);
    void joinComponents(int32_t comp1, int32_t comp2);
    int32_t findNodeConnectedComponentID(uint32_t nodeId);
    bool areConnected(int32_t comp1, int32_t comp2);
    bool rebuildIndexes();
    bool destroyConnectedComponents();
    uint32_t lastComponent;
    void initUpdateIndex(uint32_t size);
    uint32_t getComponentCount();
    uint32_t getMergedSize();
    uint32_t mergedHits;
private:
    int32_t *ccindex_;
    int32_t ccSize_;
    uint32_t ccMax_;
    static const uint32_t ccLimit_ = 8000000;
    int32_t *updateIndex_;
    uint32_t updateIndexSize_;
    static const uint32_t updateMargin_ = 1000;
    uint32_t metricVal_;
    int32_t *merged_;
    uint32_t mergedSize_;
    uint32_t mergedMax_;
    static const int mergedInitial_ = 5000;
};

class DynamicGraph : public Graph{
public:
    DynamicGraph();
    ~DynamicGraph();
    bool postAdd(uint32_t from, uint32_t to);
    void estimateConnectedComponents();
    CC* getCC();
    long query(uint32_t from, uint32_t to);
private:
    CC* connectedComponents_;
    Hash<uint32_t> visited_;
    static const uint32_t visitedSize_ = 11087;
};


#endif
