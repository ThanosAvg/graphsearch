#ifndef SCC_H
#define SCC_H

#include <stdint.h>

struct Component{
    uint32_t component_id; //current component id
    uint32_t included_nodes_count; //number of nodes in component
    uint32_t* included_node_ids;//ids of included nodes

    uint32_t* componentNeighborsOutgoing;
    uint32_t neighborNumberOutgoing;
    uint32_t neighborMatrixSizeOutgoing;

    uint32_t* componentNeighborsIncoming;
    uint32_t neighborNumberIncoming;
    uint32_t neighborMatrixSizeIncoming;
};

struct ComponentCursor{
    Component* component_ptr; // pointer to current’s iteration component...
    // Any other necessary information in order to move to next component in the vector
};

class SCC{
public:
    SCC(uint32_t);
    ~SCC();

    uint32_t findNodeStronglyConnectedComponentID(uint32_t);
    bool next_StronglyConnectedComponentID(ComponentCursor*);
    void setComponent(Component*);
    Component* getComponent(uint32_t);
    uint32_t getComponentsCount();
    void setStronglyConnectedComponentID(uint32_t,uint32_t);

private:
    static const uint32_t initialCmatrixLength_ = 1024;

    Component** components_;
    uint32_t componentsCount_;
    uint32_t* idBelongsToComponent_;
    uint32_t componentMatrixLength_;
};

#endif
