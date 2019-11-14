#include <stdlib.h>
#include <stdio.h>
#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

Task* nextTask(){
    struct node* nextNode = waitingHead;
    if (nextNode == NULL)return NULL; 
    while (nextNode->next != NULL)nextNode = nextNode->next;
    return nextNode->task;
}

void schedule(){
    schedule_is_rr(0);
}