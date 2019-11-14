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

int main(void){
    // create some work to do
    struct data work[100];
    pool_init();

    for (int i=0; i<50; ++i){
        usleep(10000);
        work[i].a = i;
        work[i].b = i;
        if (pool_submit(&add,&work[i])){
            printf("提交任务%d失败!\n", i);
        }else{
             printf("提交任务%d成功!\n", i);
        }
    }

    sleep(3);

    for (i=50; i<100; ++i){ //连续提交任务
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
