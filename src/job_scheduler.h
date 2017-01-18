#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "query_array.h"
#include "job.h"
#include "graph.h"
#include <pthread.h>

#define JOB_END UINT_MAX-2

class JobScheduler{
public:
    JobScheduler(QueryArray* queryArray, StaticGraph* graph);
    ~JobScheduler();

    void parallelQueryExecution();
    void* threadJobExecution();
    Job* getJob();
private:
    static const int threadPoolSize_ = 4;
    pthread_t thread_pool_[threadPoolSize];
    QueryArray* queryArray_;
    StaticGraph* graph_;
    pthread_mutex_t job_mutex_;
    uint32_t nextJobStart_;
    uint32_t nextJobEnd_;
    uint32_t nextJob_;
    uint32_t jobNumber_;
};

#endif
