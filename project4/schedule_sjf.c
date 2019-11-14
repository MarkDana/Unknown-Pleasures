#include <stdlib.h>
#include <stdio.h>
#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

//maintain two lists to store tasks
struct node* waitingHead = NULL;
struct node* finishedHead = NULL;
Task* curTask = NULL;

int curTime = 0;
int runTime = 0; //for the curTask

int turnaroundTime = 0;
int waitingTime = 0;
int responseTime = 0;

int value = 1;  //used for tid
int numTasks = 0;  //

void add(char *name, int priority, int burst){
    Task* newtask = malloc(sizeof(Task));
    newtask->tid = __sync_fetch_and_add(&value,1);
    //atomically increment an integer value, atomic integer, to fix race condition

    newtask->name = name;
    newtask->priority = priority;
    newtask->burst = burst;
    newtask->lastOut = 0;
    insert(&waitingHead,newtask);
}

Task* nextTask(){  //fcfs, from tail
    struct node* nextNode = waitingHead;
    struct node* minNode = waitingHead; 
    if (nextNode == NULL)return NULL; 
    while (nextNode->next != NULL){
        if (nextNode->next->task->burst < minNode->task->burst)minNode = nextNode->next;
        nextNode = nextNode->next;
    }
    return minNode->task;
}

void updateTask(){
    curTask->burst -= runTime;
    if(curTask->burst>0){
        curTask->lastOut = curTime;
        delete(&waitingHead,curTask);
        insert(&waitingHead,curTask);
    }
    else{
        delete(&waitingHead,curTask);
        insert(&finishedHead,curTask);
        turnaroundTime += curTime;
    }
}


void schedule(){
    while (1){
        curTask = nextTask();
        if(curTask == NULL)break;
        waitingTime += (curTime - curTask->lastOut);
        if(curTask->lastOut == 0)responseTime += curTime;
        runTime = curTask->burst;
        run(curTask, runTime);
        for (int t=0;t<runTime;++t) ++curTime;
        updateTask();
    }

    struct node* fns = finishedHead;
    while(fns != NULL){
        ++numTasks;
        fns = fns->next;
    }
    printf("\n");
    printf("Average turnaround time: %fs\n",(float)turnaroundTime/numTasks);
    printf("Average waiting time: %fs\n",(float)waitingTime/numTasks);
    printf("Average response time: %fs\n",(float)responseTime/numTasks);
}