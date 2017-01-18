#include "job_scheduler.h"
#include <iostream>

JobScheduler::JobScheduler(QueryArray* queryArray,StaticGraph* graph){
    this->queryArray_=queryArray;
    this->nextJobStart_=0;
    this->nextJobEnd_=queryArray->getBurst(0);
    this->nextJob_=1;
    this->jobNumber_=queryArray->getCurrentBurstSize();
    this->graph_=graph;
}

void JobScheduler::parallelQueryExecution(){

    pthread_mutex_init(&this->job_mutex_,NULL);
    for(int i=0;i<this->threadNumber_;i++){
        if(pthread_create(&(this->thread_pool[i]), NULL, this->threadJobExecution, NULL)) {
            return;
        }
    }
    for(int i=0;i<this->threadNumber_;i++)
       pthread_join(this->thread_pool[i], NULL);

    uint32_t queryNumber=this->queryArray_->getCurrentQueryDataSize();
    for(int i=0;i<queryNumber;i++)
        this->queryArray_->printResult(i);
}

void* JobScheduler::threadJobExecution(){
    //Basic routine for the execution of queries in threads

    struct Job* job;
    QueryData* queryDataPtr;
    long result;
    while(1){
        //Get my next job from the job array
        job=this->getJob();
        if(job->to==JOB_END){
            delete job;
            break;
        }
        //Iterate over all queries of the Job
        for(uint32_t i=job->from;i<=job->to;i++){
            queryDataPtr=this->queryArray_->getQuery(i);
            result=this->graph_->staticQuery(queryDataPtr->queryFrom,queryDataPtr->queryTo);
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
    if(this->nextJobEnd_!=JOB_END){
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

JobScheduler::~JobScheduler(){

}
