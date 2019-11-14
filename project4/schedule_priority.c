#include <stdlib.h>
#include <stdio.h>
#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

Task* nextTask(){
    struct node* nextNode = waitingHead;
    struct node* priorNode = waitingHead; 
    if (nextNode == NULL)return NULL; 
    while (nextNode->next != NULL){ //<=, if times equal, pick the first come
        if (nextNode->next->task->priority >= priorNode->task->priority)priorNode = nextNode->next;
        nextNode = nextNode->next;
    }
    return priorNode->task;
}


void schedule(){
    schedule_is_rr(0);
}