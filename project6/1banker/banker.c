#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define SIZE 100
int min(int a,int b){return a<b?a:b;}

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];
/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* current available resources */
int work[NUMBER_OF_RESOURCES];
/* whether a thread can be finished */
int finish[NUMBER_OF_CUSTOMERS]; 

int request[NUMBER_OF_RESOURCES];
int release[NUMBER_OF_RESOURCES];

int safe(int customer, int request[]){  //m*n^2 operations to judge
	for (int i = 0; i < NUMBER_OF_RESOURCES; ++i)work[i] = available[i];
	for (int i = 0; i < NUMBER_OF_CUSTOMERS; ++i)finish[i] = 0;
	int ind = 0;
	while (ind < NUMBER_OF_CUSTOMERS){
		if (!finish[ind]){
			int jump = 0;
			for (int i = 0; i < NUMBER_OF_RESOURCES; ++i){
				if (need[ind][i] > work[i]){jump = 1;break;}
			}
			if (jump){
				++ ind;
				continue;
			}else{
				for(int i = 0; i < NUMBER_OF_RESOURCES; ++i)work[i] += allocation[ind][i];
				finish[ind] = 1;
				ind = 0;
				continue;
			}
		}
		++ ind;
	}
	for(ind = 0; ind < NUMBER_OF_CUSTOMERS; ++ind)if(!finish[ind])return 0;
	return 1;
}

void tryAllocate(int customer, int request[]){
	for(int i=0; i<NUMBER_OF_RESOURCES; ++i){
		available[i] -= request[i];
		need[customer][i] -= request[i];
		allocation[customer][i] += request[i];
	}
}

void release_resources(int customer, int release[]){
	int canRelease;  //avoid releasing more than it has been allocated
	for(int i = 0; i < NUMBER_OF_RESOURCES; ++i){
		canRelease = min(release[i], allocation[customer][i]);
		available[i] += canRelease;
		need[customer][i] += canRelease;
		allocation[customer][i] -= canRelease;
	}
}

int request_resources(int customer, int request[]){
	int issafe;
	for(int i=0; i<NUMBER_OF_RESOURCES; ++i){
		if(request[i] > need[customer][i]){
			printf("FAILED: resource %d remaining for customer %d is %d, less than %d\n", i, customer, need[customer][i], request[i]);
			return -1;
		}
		if(request[i] > available[i]){
			printf("FAILED: only %d resource %d available, less than %d\n", available[i], i, request[i]);
			return -1;
		}	
	}
	tryAllocate(customer, request); //try to alloacate, then examine
	issafe = safe(customer,request);
	if(issafe) return 0;
	else{
		release_resources(customer, request);
		printf("FAILED: state not safe after allocating to customer %d\n",customer);
		return -1;
	}
}

void printState(){
	printf("Available:\n");
	for(int i = 0; i<NUMBER_OF_RESOURCES; ++i)printf("%d\t",available[i]);
	printf("\nAllocation:\n");
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
		for(int j = 0; j < NUMBER_OF_RESOURCES; ++j)printf("%d\t",allocation[i][j]);
		printf("\n");
	}
	printf("\nNeed:\n");
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
		for(int j = 0; j < NUMBER_OF_RESOURCES; ++j)printf("%d\t",need[i][j]);
		printf("\n");
	}
	printf("\n");
}

int main(int argc, char *argv[]){   
	printf("%d resources initial num: ", NUMBER_OF_RESOURCES);
	for (int i=0; i<NUMBER_OF_RESOURCES; ++i){
		available[i] = atoi(argv[i + 1]);
		printf("%d\t",available[i]);}
	printf("\n");
	
	FILE *in;
    char *temp;
    char costm[SIZE];
    int ind = 0;
	in = fopen("./max.txt","r");
    printf("Reading file to get maximum allocation\n");
	while (fgets(costm,SIZE,in) != NULL) {
        temp = strdup(costm);
        for (int i = 0; i < NUMBER_OF_RESOURCES; ++i){
        	maximum[ind][i] = min(available[i], atoi(strsep(&temp,","))); //avoid max > available
			allocation[ind][i] = 0;
			need[ind][i] = maximum[ind][i];  //max-allocation
        }
        free(temp);
        ++ind;
    }
    fclose(in);
    printf("\nMax:\n");
	for(int i = 0; i < NUMBER_OF_CUSTOMERS; ++i){
		for(int j = 0; j < NUMBER_OF_RESOURCES; ++j)printf("%d\t",need[i][j]);
		printf("\n");
	}
	printf("\n");

    char command[10];
    int thiscustomer;

    while (1) {
		printf("Please input command, e.g. RQ 0 3 1 2 1 / RL 4 1 2 3 1 / *");
		scanf("%s", command);
		if(strcmp(command,"RQ")==0){
			scanf("%d", &thiscustomer);
			for(int i=0; i < NUMBER_OF_RESOURCES; ++i)scanf("%d", &request[i]);
			if(request_resources(thiscustomer,request) == 0)printf("SUCCEEDED: customer %d requests resources\n",thiscustomer);
		}
		if(strcmp(command,"RL")==0){
			scanf("%d", &thiscustomer);
			for(int i=0; i < NUMBER_OF_RESOURCES; ++i)scanf("%d", &release[i]);
			release_resources(thiscustomer,release);
		 	printf("SUCCEEDED: customer %d releases resources\n",thiscustomer);
		}
		if(strcmp(command,"*")==0){
			printState();
		}
		fflush(stdin);
	}
}