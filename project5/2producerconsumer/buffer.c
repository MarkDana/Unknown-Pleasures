#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"
#include <time.h>
int randomM(int m){return rand()%m;}
const int MAX = 1001;  //max 1ms

buffer_item buffer[BUFFER_SIZE];
int head, tail;

pthread_mutex_t mutex;  //binary semaphore
sem_t empty;
sem_t full;

int num_producer,num_consumer;
pthread_t *producer_bee,*consumer_bee;

int insert_item(buffer_item item, int producer_id){//no need to return -1 indicating failure
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    // if (sizenow == BUFFER_SIZE){
    //     pthread_mutex_unlock(&mutex);
    //     sem_post(&empty);
    //     return -1;
    // }
    buffer[tail] = item;
    tail = (tail + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    printf("producer %d -> item %d -> buffer\n",producer_id, item);
    return 0;
}

int remove_item(buffer_item *item, int consumer_id){
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    *item = buffer[head];
    head = (head + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    printf("consumer %d <- item %d <- buffer\n",consumer_id, *item);
    return 0;
}

void *producer(void *param){ 
    int producer_id = *(int *)param;
    buffer_item item;
    while (1){
        usleep(randomM(MAX));
        item=randomM(MAX);
        insert_item(item, producer_id);
    }
    pthread_exit(0);
}

void *consumer(void *param){
    int consumer_id = *(int *)param; 
    buffer_item item;
    while(1){
        usleep(randomM(MAX));
        remove_item(&item, consumer_id);
    }
    pthread_exit(0);
}

void pool_init(int nump, int numc){
    srand(time(NULL));
    head = 0;
    tail = 0;
    num_producer = nump;
    num_consumer = numc;
    producer_bee = (pthread_t*)malloc(sizeof(pthread_t) *num_producer);
    consumer_bee = (pthread_t*)malloc(sizeof(pthread_t) *num_consumer);
    
    for(int i=0; i<num_producer; ++i)pthread_create(&producer_bee[i], NULL, producer, &i);
    for(int i=0; i<num_consumer; ++i)pthread_create(&consumer_bee[i], NULL, consumer, &i);
        
    sem_init(&full,0,0);
    sem_init(&empty,0,BUFFER_SIZE);
    pthread_mutex_init(&mutex,NULL);
}

void pool_shutdown(void){
    for (int i=0; i<num_producer; ++i){
        pthread_cancel(producer_bee[i]);
        pthread_join(producer_bee[i],NULL);
    }
    for (int i=0; i<num_consumer; ++i){
        pthread_cancel(consumer_bee[i]);
        pthread_join(consumer_bee[i],NULL);
    }
}