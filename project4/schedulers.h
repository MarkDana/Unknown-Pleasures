#define MIN_PRIORITY 1
#define MAX_PRIORITY 10
#include "task.h"

struct node* waitingHead;
struct node* finishedHead;
Task* curTask;

int curTime;
int runTime; //for the curTask

int turnaroundTime;
int waitingTime;
int responseTime;

int value;  //used for tid
int numTasks;  //

// add a task to the list 
void add(char *name, int priority, int burst);

// invoke the scheduler
void schedule();
void schedule_is_rr(int isrr);
Task* nextTask();
