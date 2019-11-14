#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define random(x) (rand()%x) 
//顶一个函数生成随机数
#include<unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"

#define true 1
#define Mod 1000000 
buffer_item buffer[BUFFER_SIZE];   // 缓冲
pthread_mutex_t mutex;  // 互斥锁
sem_t empty;  //两个信号量
sem_t full;
int in,out; //循环队列的头和尾
int producer_number,consumer_number; //生产者和消费者的数目
pthread_t *p,*c;//指向生产者数组和消费者数组的指针
int producer_consumer_init(int Producer_number,int Consumer_number){
    in=0;out=0; //设置循环队列的头和尾
    producer_number=Producer_number; //设置生产者和消费者的数目
    consumer_number=Consumer_number;

    p=(pthread_t*)malloc(sizeof(pthread_t)*producer_number);//为生产者和消费者pthread_t数组分配空间
    c=(pthread_t*)malloc(sizeof(pthread_t)*consumer_number);
    
    for(int i=0;i<producer_number;++i){ //初始化生产者
        int j= pthread_create(&p[i], NULL, producer, NULL);
        if(j==0) printf("创建[生产者线程%d]成功!\n",i);
        else{
                printf("创建[生产者线程%d]失败,终止执行!\n",i);
                return 1;
        }
    }

    for(int i=0;i<consumer_number;++i){ //初始化消费者
        int j= pthread_create(&c[i], NULL, consumer, NULL);
        if(j==0) printf("创建[消费者线程%d]成功!\n",i);
        else{
                printf("创建[消费者线程%d]失败,终止执行!\n",i);
                return 1;
        }
    }

    sem_init(&full,0,0); //把full设置为0，表明目前缓冲为空
    sem_init(&empty,0,BUFFER_SIZE);//把empty设置为BUFFER_SIZE，表明缓冲为空
    pthread_mutex_init(&mutex,NULL); //初始化互斥锁

    return 0;
}

void producer_consumer_term(){
      for(int i=0;i<producer_number;++i){ //终止生产者线程
        pthread_cancel(p[i]);
        int j=pthread_join(p[i], NULL);   
        if(j==0) printf("终止[生产者线程%d]成功!\n",i);
    }

    for(int i=0;i<consumer_number;++i){//终止消费者线程
        pthread_cancel(c[i]);
        int j=pthread_join(c[i], NULL);   
        if(j==0) printf("终止[消费者线程%d]成功!\n",i);
       
    }
}

int insert_item(buffer_item item)
{
    buffer[in] = item; //插入物品
    printf("生产者生产了物品[%d],放入缓冲的[%d]位置.\n",item,in); //输出一些信息
    in=(in+1)% BUFFER_SIZE;//更新循环队列的头
    return 0;
}

int remove_item(buffer_item *item)
{
    *item = buffer[out]; //移出物品
    printf("消费者消费了物品[%d],从缓冲的[%d]位置移出.\n",*item,out); //输出一些信息
    out=(out+1)% BUFFER_SIZE;//更新循环队列的尾
    return 0;
}

void *producer(void *param)
{   
    buffer_item item;
    while (true){
        usleep(random(Mod)); //随机休眠一段时间
        item=random(Mod); //产生生产的物品
        sem_wait(&empty); //减小empty的数值
        pthread_mutex_lock(&mutex); //获得互斥锁
            insert_item(item); //添加物品
        pthread_mutex_unlock(&mutex);
        sem_post(&full);//增加full的数值
    }
    pthread_exit(0);
}

void *consumer(void *param)
{
    buffer_item item;
    while(true){
        usleep(random(Mod)*5); //消费者的休眠时间平均上比生产者长一点，因为我这里的生产者数量少，消费者数量多
        sem_wait(&full); //减少full的值
        pthread_mutex_lock(&mutex);//获得互斥锁
            remove_item(&item); //移除物品
        pthread_mutex_unlock(&mutex);//释放互斥锁
        sem_post(&empty);//增加empty的值
    }
    pthread_exit(0);
}