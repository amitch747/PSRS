#include "threadpool.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


ThreadPool_t *ThreadPool_create(unsigned int num) {
    ThreadPool_t *pool = (ThreadPool_t*)malloc(sizeof(ThreadPool_t)); //Create pool object
    pool->threads = (pthread_t*)malloc(num* sizeof(pthread_t)); //Create array for num threads
    pool->jobs.head = NULL;
    pool->jobs.size = 0;
    pool->num_threads = num;
    pool->end = false;

    pool->idle_threads = (bool*)malloc(num * sizeof(bool));
    for (unsigned int i = 0; i < num; i++) {
        pool->idle_threads[i] = true;  // Initially, all threads are idle
    }


    for(int i=0; i<num; i++) {
        if(pthread_create(&pool->threads[i], NULL, &Thread_run, pool) != 0) {
            //printf("Failed to create thread %d\n",i);
        }
        else{
            //printf("Thread create\n");
        }
    }
    return pool;
}


void *Thread_run(void *arg) {
    ThreadPool_t *tp = (ThreadPool_t *) arg;
    ThreadPool_job_t *job = NULL; // Every thread gets a job

    int thread_index;

    for (int i = 0; i < tp->num_threads; i++) {
        if (pthread_equal(pthread_self(), tp->threads[i])) {
            thread_index = i;
            break;
        }
    }
    //printf("Threadindex:%d\n",thread_index);
    while(1) {
        pthread_mutex_lock(&mutexJobs);
        
        //printf("end:%d...size:%d\n",tp->end,tp->jobs.size);
        if (tp->end) { //Threads passing by this will be killed if end is true
            //printf("Breaking\n");
            pthread_mutex_unlock(&mutexJobs);
            break; // Time to die
        }
        
        while (tp->jobs.size == 0 && !tp->end) {
            //printf("Waiting\n");
            tp->idle_threads[thread_index] = true;
            pthread_cond_wait(&condJobs, &mutexJobs);
        }
         tp->idle_threads[thread_index] = false; 
        //printf("Done waiting\n");
        job = ThreadPool_get_job(tp);
        //printf("Job assigned to thread[%d]\n",thread_index);
        pthread_mutex_unlock(&mutexJobs);
        if (job != NULL)
        {
            //printf("No way\n");
            thread_func_t func = job->func;
            void *arg = job->arg;
            func(arg);
            free(job);
            //printf("Threadloop\n");
        }
        tp->idle_threads[thread_index] = true;


    }
    return NULL;
}