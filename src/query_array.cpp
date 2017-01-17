#include <stdlib.h>
#include <string.h>

#include "query_array.h"

QueryArray::QueryArray(uint32_t initialQueryDataSize, uint32_t initialBurstDataSize){
    dataArray_ = (QueryData *) malloc(sizeof(QueryData) * initialQueryDataSize);
    memset(dataArray_, 0, initialQueryDataSize);
    maxDataSize_ = initialQueryDataSize;
    currentDataSize_ = 0;

    burstArray_ = (uint32_t *) malloc(sizeof(uint32_t) * initialBurstDataSize);
    maxBurstSize_ = initialBurstDataSize;
    currentBurstSize_ = 0;
}

QueryArray::~QueryArray(){
    delete(dataArray_);
    delete(burstArray_);
}

bool QueryArray::addQuery(uint32_t from, uint32_t to, uint32_t id, uint32_t version){
    if(currentDataSize_ >= maxDataSize_){
        QueryData* resized;
        resized = (QueryData *) realloc(dataArray_, maxDataSize_ * sizeof(QueryData) * 2);
        if(resized == NULL){
            // Realloc failure
            free(dataArray_);
            return false;
        }
        dataArray_ = resized;
        maxDataSize_ = maxDataSize_ * 2;
    }

    dataArray_[currentDataSize_].queryFrom = from;
    dataArray_[currentDataSize_].queryTo = to;
    dataArray_[currentDataSize_].queryId = id;
    dataArray_[currentDataSize_].version = version;
    currentDataSize_++;
    return true;
}

QueryData* QueryArray::getQuery(uint32_t index){
    return &(dataArray_[index]);
}

uint32_t QueryArray::getCurrentQueryDataSize(){
    return currentDataSize_;
}

bool QueryArray::addBurst(uint32_t line){
    if(currentBurstSize_ >= maxBurstSize_){
        uint32_t* resized;
        resized = (uint32_t *) realloc(burstArray_, currentBurstSize_ * sizeof(uint32_t) * 2);
        if(resized == NULL){
            // Realloc failure
            free(burstArray_);
            return false;
        }
        burstArray_ = resized;
        maxBurstSize_ = maxBurstSize_ * 2;
    }
    burstArray_[currentBurstSize_] = line;
    currentBurstSize_++;
    return true;
}

uint32_t* QueryArray::getBurst(uint32_t index){
    return &(burstArray_[index]);
}

uint32_t QueryArray::getCurrentBurstSize(){
    return currentBurstSize_;
}


