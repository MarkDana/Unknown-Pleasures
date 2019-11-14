/**
 * Representation of a task in the system.
 */

#ifndef TASK_H
#define TASK_H   
//上面是一个空的宏定义 目的就是防止下面的结构体重复定义

// representation of a task
typedef struct task {
    char *name; //任务名
    int tid; 
    int priority; //优先级
    int burst; //CPU区间
    int arrival_time; //到达时间
    int last_join_time; //上一次进入调度队列的时间
    int response_time; //响应时间
    int waiting_time;//当前等待时间
    int last_out_time;//上一次离开调度队列的时间
    int turnaround_time;//周转时间
} Task;

#endif
