#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "query_array.h"
#include "job.h"
#include "graph.h"
#include "types.h"
#include <pthread.h>

class JobScheduler{
public:
    JobScheduler(QueryArray* queryArray, StaticGraph* graph);
    ~JobScheduler();

    void parallelQueryExecution();
    static void* staticFunctionPointer(void * argp);
    void threadJobExecution();
    Job* getJob();
private:
    static const int threadPoolSize_ = 4;
    pthread_t threadPool_[threadPoolSize_];
    QueryArray* queryArray_;
    StaticGraph* graph_;
    pthread_mutex_t job_mutex_;
    uint32_t nextJobStart_;
    uint32_t nextJobEnd_;
    uint32_t nextJob_;
    uint32_t jobNumber_;
};

#endif
