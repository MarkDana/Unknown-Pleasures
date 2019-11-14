/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    int i;
    struct data work[200];
    

   
    i=pool_init(); //初始化线程池
    if(i!=0) 
        return 0; //初始化失败
    sleep(3);


  
    for (i=0; i<50; ++i){ //提交50个任务
        usleep(10000); //每隔10000us提交一次任务,即10ms
        work[i].a = i;
        work[i].b = i;
        if (pool_submit(&add,&work[i])){
            printf("提交任务%d失败!\n", i);
        }else{
             printf("提交任务%d成功!\n", i);
        }
    }
    

    sleep(3);

     for (i=50; i<200; ++i){ //连续提交任务
        work[i].a = i;
        work[i].b = i;
        if (pool_submit(&add,&work[i])){
            printf("提交任务%d失败!'\n", i);
        }else{
             printf("提交任务%d成功!'\n", i);
        }
    }

    sleep(3);
    pool_shutdown(); //关闭线程池

    return 0;
}