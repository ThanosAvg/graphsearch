#ifndef QUERY_ARRAY_H
#define QUERY_ARRAY_H

#include <stdint.h>

struct QueryData{
    uint32_t queryFrom;
    uint32_t queryTo;
    uint32_t queryId;
    uint32_t version;
};


class QueryArray{
public:
    QueryArray(uint32_t initialQueryDataSize, uint32_t initialBurstDataSize);
    ~QueryArray();
    bool addQuery(uint32_t from, uint32_t to, uint32_t id, uint32_t version);
    QueryData* getQuery(uint32_t index);
    uint32_t getCurrentQueryDataSize();

    bool addBurst(uint32_t line);
    uint32_t* getBurst(uint32_t index);
    uint32_t getCurrentBurstSize();
private:
    QueryData* dataArray_;
    uint32_t maxDataSize_;
    uint32_t currentDataSize_;

    uint32_t* burstArray_;
    uint32_t maxBurstSize_;
    uint32_t currentBurstSize_;
};
#endif
