#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "buffer.h" 

int main(int argc, char *argv[]){   
    int sleep_time = atoi(argv[1]);
    int nump = atoi(argv[2]);
    int numc = atoi(argv[3]);
    pool_init(nump, numc);
    sleep(sleep_time); //seconds
    pool_shutdown();
    return 0;
}