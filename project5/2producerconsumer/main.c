#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "buffer.h" 

int main(int argc, char *argv[]){
	if (strcmp(argv[1],"-h")==0){
		printf("command like ./example 10 5 10  (10s, 5 producers, 10 consumers)\n");
		return 0;}  
    int sleep_time = atoi(argv[1]);
    int nump = atoi(argv[2]);
    int numc = atoi(argv[3]);
    pool_init(nump, numc);
    sleep(sleep_time); //seconds
    pool_shutdown();
    return 0;
}