#include <stdio.h>
#include <stdlib.h>

typedef int buffer_item;
#define BUFFER_SIZE 5

int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void *producer(void *param);
void *consumer(void *param);
void producer_consumer_term();
int producer_consumer_init(int Producer_number,int Consumer_number);