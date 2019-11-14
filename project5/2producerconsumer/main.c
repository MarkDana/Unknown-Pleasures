#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "buffer.h" 

int main(int argc, char *argv[]){   
    sleep_time = atoi(argv[1]);
    nump = atoi(argv[2]);
    numc = atoi(argv[3]);
    pool_init(int nump, int numc);
    usleep(sleep_time);
    pool_shutdown();
    return 0;
}