/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
task worktodo[QUEUE_SIZE];
int head, tail;
int isPoolOpen;
int sizenow;
sem_t semSize;

pthread_mutex_t lock;

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    pthread_mutex_lock(&lock); //get lock
    if (sizenow == QUEUE_SIZE || !isPoolOpen){
        pthread_mutex_unlock(&lock);
        return 1; //fail
    }
    worktodo[tail] = t;
    tail = (tail + 1) % QUEUE_SIZE;
    ++sizenow;
    pthread_mutex_unlock(&lock);
    sem_post(&semSize);
    return 0;
}

// remove a task from the queue
task dequeue() {
    task work;
    sem_wait(&semSize);  //acquire semaphore
    pthread_mutex_lock(&lock);
    work = worktodo[head];
    head = (head + 1) % QUEUE_SIZE;
    --sizenow;
    pthread_mutex_unlock(&lock);
    return work;
}

// the worker thread in the thread pool
void *worker(void *param){
    task work;
    while (TRUE){
        work = dequeue();
        execute(work.function, work.data);
    }
    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p){
    task work;
    work.function = somefunction;
    work.data = p;
    return enqueue(work);//0 if successful
}

// initialize the thread pool
void pool_init(void){
    head = 0;
    tail = 0;
    sizenow = 0;
    isPoolOpen = 1;
    sem_init(&semSize,0,0);  //init as 0
    pthread_mutex_init(&lock,NULL);
    for (int i=0; i<NUMBER_OF_THREADS; ++i){
        pthread_create(&bee[i], NULL, worker, NULL);
}

// shutdown the thread pool
void pool_shutdown(void){
    isPoolOpen = 0;
    for (int i=0; i<NUMBER_OF_THREADS; ++i){
        pthread_cancel(bee[i]);
        pthread_join(bee[i],NULL);
    }
}
