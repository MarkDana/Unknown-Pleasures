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

    printf("Now submit tasks with interval 5ms");
    for (int i=0; i<50; ++i){
        usleep(5000);  //5ms
        work[i].a = i;
        work[i].b = i;
        if (pool_submit(&add,&work[i]))printf("submitting task%d failed\n", i);
        else printf("submitting task%d succeeded\n", i);
    }

    printf("Now submit tasks with no interval time");

    for (int i=50; i<100; ++i){
        work[i].a = i;
        work[i].b = i;
        if (pool_submit(&add,&work[i]))printf("submitting task%d failed\n", i);
        else printf("submitting task%d succeeded\n", i);
    }

    pool_shutdown();
    return 0;
}
