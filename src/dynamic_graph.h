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
    void addNodeToComponent(uint32_t nodeId, uint32_t ccId);
    void postAddNodeToComponent(uint32_t nodeId, uint32_t ccId);
    //bool insertNewEdge(uint32_t nodeIdS, uint32_t nodeIdE);
    void joinComponents(uint32_t comp1, uint32_t comp2);
    int32_t findNodeConnectedComponentID(uint32_t nodeId);
    bool areConnected(uint32_t comp1, uint32_t comp2);
    bool rebuildIndexes();
    bool destroyConnectedComponents();
    uint32_t lastComponent;
    void initUpdateIndex(uint32_t size);
    uint32_t getComponentCount();
private:
    uint32_t *ccindex_;
    uint32_t ccSize_;
    uint32_t ccMax_;
    static const uint32_t ccLimit_ = 10000000;
    //Hash<uint32_t> ccindex_;
    //Hash<uint32_t> updateIndex_;
    ConnectData *updateIndex_;
    uint32_t updateIndexSize_;
    static const uint32_t updateLimit_ = 4000;
    uint32_t metricVal_;
    static const int bucketNIndex_ = 37;
    static const int bucketNUpdate_ = 37;
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
