#include "node_index.h"

class Graph{
public:
    Graph();
    void add(int from, int to);
private:
    NodeIndex* incoming;
    NodeIndex* outcoming;
};
