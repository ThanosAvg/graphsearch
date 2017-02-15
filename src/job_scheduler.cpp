#include "job_scheduler.h"
#include "types.h"
#include <iostream>

using namespace std;

JobScheduler::JobScheduler(QueryArray* queryArray,Graph* graph){
    this->queryArray_=queryArray;
    this->nextJobStart_=0;
    this->nextJobEnd_=queryArray->getBurst(0);
    this->nextJob_=1;
    this->jobNumber_=queryArray->getCurrentBurstSize();
    this->graph_=graph;
}

void JobScheduler::parallelQueryExecution(){

    pthread_mutex_init(&this->job_mutex_,NULL);
    for(int i=0;i<this->threadPoolSize_;i++){
        if(pthread_create(&(this->threadPool_[i]), NULL, JobScheduler::staticFunctionPointer, this)) {
            return;
        }
    }
    for(int i=0;i<this->threadPoolSize_;i++)
       pthread_join(this->threadPool_[i], NULL);

    uint32_t queryNumber=this->queryArray_->getCurrentQueryDataSize();
    for(int i=0;i<queryNumber;i++)
        this->queryArray_->printResult(i);
}

void * JobScheduler::staticFunctionPointer(void* argp){
    ((JobScheduler *) argp)->threadJobExecution();
    return NULL;
}

void JobScheduler::threadJobExecution(){
    //Basic routine for the execution of queries in threads

    struct Job* job;
    QueryData* queryDataPtr;
    long result;

    uint32_t startVisitedSize=this->graph_->getOutgoingIndexSize();
    uint32_t endVisitedSize=this->graph_->getIncomingIndexSize();
    uint32_t* startVisited=(uint32_t*)calloc(startVisitedSize,sizeof(uint32_t));
    uint32_t* endVisited=(uint32_t*)calloc(endVisitedSize,sizeof(uint32_t));
    uint32_t startVisitedKey=0;
    uint32_t endVisitedKey=0;

    while(1){
        //Get my next job from the job array
        job=this->getJob();
        if(job->to == JOB_END){
            delete job;
            break;
        }
        //Iterate over all queries of the Job
        for(uint32_t i=job->from;i<=job->to;i++){
            queryDataPtr=this->queryArray_->getQuery(i);

            if(startVisitedKey==UINT_MAX){
                memset(startVisited,0,startVisitedSize);
                startVisitedKey=0;
            }
            if(endVisitedKey==UINT_MAX){
                memset(endVisited,0,endVisitedSize);
                endVisitedKey=0;
            }
            startVisitedKey++;
            endVisitedKey++;
            result=this->graph_->threadSafeQuery(queryDataPtr->queryFrom,
                                                 queryDataPtr->queryTo,startVisitedKey,startVisited,endVisitedKey,endVisited,queryDataPtr->version);
            //Write result to the result Array
            this->queryArray_->setResult(i,result);
        }
        delete job;
    }
}

Job* JobScheduler::getJob(){
    //Give next job to a working thread

    struct Job* newJob=new Job();
    pthread_mutex_lock(&job_mutex_);
    newJob->from=this->nextJobStart_;
    newJob->to=this->nextJobEnd_;
    if(this->nextJobEnd_ != JOB_END){
        if(this->nextJob_==this->jobNumber_){
            this->nextJobStart_=JOB_END;
            this->nextJobEnd_=JOB_END;
        }
        else{
            this->nextJobStart_=this->nextJobEnd_+1;
            this->nextJobEnd_=this->queryArray_->getBurst(this->nextJob_);
            this->nextJob_++;
        }
    }
    pthread_mutex_unlock(&job_mutex_);
    return newJob;
}
