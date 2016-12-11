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
    GrailIndex(uint32_t);
    ~GrailIndex();

    uint32_t getMinRank(int,uint32_t);
    uint32_t getRank(int,uint32_t);
    uint32_t setMinRank(int,uint32_t, uint32_t);
    uint32_t setRank(int,uint32_t, uint32_t);
    bool isSubset(uint32_t, uint32_t);
    static const uint32_t labels = 1;

private:
    grailNode** index_;
    uint32_t size_;
};

#endif
