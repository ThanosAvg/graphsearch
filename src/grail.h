#ifndef GRAIL_H
#define GRAIL_H

#include <stdint.h>

enum GrailAnswer{NO, MAYBE, YES};

struct grailNode{
    uint32_t minRank;
    uint32_t rank;
};

class GrailIndex{
public:
    GrailIndex(uint32_t indexSize);
    ~GrailIndex();

    uint32_t getMinRank(int label, uint32_t indexPosition);
    uint32_t getRank(int label, uint32_t indexPosition);
    uint32_t setMinRank(int label, uint32_t indexPosition, uint32_t minRank);
    uint32_t setRank(int label, uint32_t indexPosition, uint32_t rank);
    bool isSubset(uint32_t source, uint32_t target);
    static const uint32_t labels = 4;

private:
    grailNode** index_;
    uint32_t size_;
};

#endif
