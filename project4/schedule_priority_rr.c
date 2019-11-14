#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "task.h"
#include "cpu.h"

const int INF=0x7fffffff;
struct node *head=NULL; //调度队列的头结点
struct node *finished=NULL; //已经完成的任务队列的头结点
Task* curTask=NULL; //指向当前任务的指针
int curTime=0; //现在的时间
int nextTime=0; //下一次事件的时间(即当前任务结束对CPU占用的时间，需要再次调度的时间)
int total_turnaround_time=0; //总周转时间
int total_waiting_time=0; //总等待时间
int total_response_time=0; //总响应时间
double average_turnaround_time=0; //平均周转时间
double average_waiting_time=0; //平均等待时间
double average_response_time=0;//平均响应时间
int value = 1;//应该赋予每个任务的pid的值
struct node *start;  //临时变量
int num;//总任务数

int min(int a,int b){ //找到最小值 时间片和CPU区间的最小值
    if(a<b)
        return a;
    else 
        return b;
}
void add(char *name, int priority, int burst){
    Task *newtask = malloc(sizeof(Task));
    newtask->tid = __sync_fetch_and_add(&value,1); //设置相关数据
    newtask->name = name;
    newtask->priority=priority;
    newtask->burst=burst;
    newtask->arrival_time=curTime;
    newtask->last_join_time=curTime;
    newtask->response_time=-1;
    newtask->waiting_time=0;
    insert(&head,newtask);
}

Task* pick_NextTask(){
    if(head==NULL) return NULL;
    int priority=0;
    struct node * nextNode = head; 
    Task* nextTask=NULL;
   
    while (nextNode!=NULL){ //寻找优先级最高的且最先加入到调度队列的任务
        if(nextNode->task->priority>=priority){
            priority=nextNode->task->priority;
            nextTask=nextNode->task;
        }
        nextNode=nextNode->next;
    }
    return nextTask;
}


void deal_curTask(){
   
       if(curTask!=NULL){
            curTask->burst-=(curTime-curTask->last_out_time);
            //printf("%d\n",curTask->burst);
            if(curTask->burst==0){ //任务已执行完
                curTask->turnaround_time=curTime-curTask->arrival_time; //计算周转时间
                delete(&head,curTask);//从调度队列删除
                insert(&finished,curTask); //插入到完成队列
                
            }else{
                curTask->last_join_time=curTime;
                delete(&head,curTask); //从调度队列删除 再插入回调度队列
                insert(&head,curTask);
                
        }
}
}
// round-robin
void schedule(){

    while(head!=NULL){
        while(curTime<nextTime) curTime++;
        deal_curTask();
        curTask=pick_NextTask();
        if(curTask==NULL) break; //全部任务执行完 退出
        curTask->last_out_time=curTime;
        curTask->waiting_time+=(curTime-curTask->last_join_time);
        if(curTask->response_time<0)
            curTask->response_time=curTime;
        nextTime=curTime+min(QUANTUM,curTask->burst);
        run(curTask, min(QUANTUM,curTask->burst));
    }

    printf("\n");
    start=finished;
    while(start!=NULL){ //统计数据
        ++num;
        total_response_time+=start->task->response_time;
        total_turnaround_time+=start->task->turnaround_time;
        total_waiting_time+=start->task->waiting_time;
        printf("[tid%d] [%s] turnaround:[%ds], waiting:[%ds], response:[%ds].\n",start->task->tid,start->task->name,start->task->turnaround_time,start->task->waiting_time,start->task->response_time);
        start=start->next;
    }
    printf("\n"); //输出数据
    printf("total turnaround:[%ds], waiting:[%ds], response:[%ds].\n",total_turnaround_time,total_waiting_time,total_response_time);
    printf("average turnaround:[%fs], waiting:[%fs], response:[%fs].\n",(double)total_turnaround_time/num,(double)total_waiting_time/num,(double)total_response_time/num);

}