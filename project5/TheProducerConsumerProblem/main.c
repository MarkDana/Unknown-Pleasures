#include<stdio.h>
#include<stdlib.h>
#include"buffer.h"
#include<unistd.h> 

// ./example "10" "3" "6" 模拟十秒 3个生产者 6个消费者
int main(int argc, char *argv[])
{   
    int sleep_time, producer_number, consumer_number;
    int i;
   
    sleep_time = atoi(argv[1]); //设置睡眠时间 生产者数目和消费者数目
    producer_number = atoi(argv[2]);
    consumer_number = atoi(argv[3]);

    i=producer_consumer_init(producer_number,consumer_number); //进行初始化
    if(i!=0) return 0;

    sleep(sleep_time); //休眠

    producer_consumer_term(); //结束模拟

    return 0;
}