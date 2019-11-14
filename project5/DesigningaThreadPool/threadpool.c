/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h> // 定义了很多宏 比如O_CREAT
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1
#define FALSE 0

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
int in, out; //循环队列的头和尾 剩余空间

sem_t full;
int empty;
pthread_mutex_t lock;
int allowed; //指示是否还允许向队列添加任务(pool_shutdown后)
// the worker bee
pthread_t bee[NUMBER_OF_THREADS];

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{   
    
    pthread_mutex_lock(&lock); //获得了互斥锁
    if (!empty || !allowed){ //但是队列已经没有空的位置 或者不允许再添加任务
        pthread_mutex_unlock(&lock); // 释放互斥锁
        return 1; //添加任务失败
    }
    worktodo[in] = t; //成功添加任务
    in = (in + 1) % QUEUE_SIZE; //更新队首
    empty--;//更新队列空位数量
    pthread_mutex_unlock(&lock); //释放互斥锁
    sem_post(&full);//更新队列内任务数量
    return 0; //添加成功
}

// remove a task from the queue
task dequeue() 
{
    task work;
    sem_wait(&full); //如果任务队列中还有任务
    pthread_mutex_lock(&lock); //获得互斥锁
    work = worktodo[out]; //获取任务
    out = (out + 1) % QUEUE_SIZE; //更新队尾任务
    empty++; //更新任务队列中的空位
    pthread_mutex_unlock(&lock); //释放互斥锁 

    return work;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    // execute the task
    task work;
    while (TRUE){
        work = dequeue(); //获取任务
        execute(work.function, work.data); //执行任务
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
int pool_submit(void (*somefunction)(void *p), void *p)
{
    task work;
    work.function = somefunction; //设置函数
    work.data = p; //设置传给函数的参数
    return enqueue(work); //尝试入队
}

// initialize the thread pool
int pool_init(void)
{
    int i,j;
    in = 0; //设置循环队列的头
    out = 0; //设置循环队列的尾
    allowed=TRUE; //允许向循环队列添加任务
    sem_init(&full,0,0); //初始化full信号量为0,表明目前队列是空的
    empty=QUEUE_SIZE;// 初始化empty为QUEUE_SIZE,表明目前队列是空的，这里不能使用信号量，否则pool_submit会一直等待
    pthread_mutex_init(&lock,NULL); //初始化互斥锁
    for (i=0; i<NUMBER_OF_THREADS; ++i){ //初始化线程池中的线程
        j= pthread_create(&bee[i], NULL, worker, NULL);
        if(j==0) printf("创建[线程%d]成功!\n",i);
        else{
                printf("创建[线程%d]失败,终止执行!\n",i); //如果有线程初始化失败,终止执行
                return 1;
        }
    }
    return 0;       
}

// shutdown the thread pool
void pool_shutdown(void)
{
    int i,j;
    allowed=FALSE; //不再允许向队列插入任务
    for (i=0; i<NUMBER_OF_THREADS; ++i){ //依次终止线程
        pthread_cancel(bee[i]);
        j=pthread_join(bee[i], NULL);   
        if(j==0) printf("终止[线程%d]成功!\n",i);
    }
    // sem_close(full); //关闭有名信号量
    // sem_unlink("FULL");//删除/dev/shm下的文件 删除有名信号量
    // //有名信号量  https://www.cnblogs.com/jfyl1573/p/6820372.html
}