#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 0x3f3f3f3f
#define MIN 0
#define CMD_LEN_MAX 20

typedef struct pcb{
	char pname [CMD_LEN_MAX];
	int base;
	int limit;
	struct pcb* next;
}PCB;

PCB *head, *tail;
//use link table to store the processes' fragments

PCB* newPCB (char* pname, int base, int limit, PCB* next){
	PCB* p = (PCB*) malloc (sizeof(PCB));
	strcpy (p->pname, pname);
	p->base = base;
	p->limit = limit;
	p->next = next;
	return p;
}

PCB* firstFit (int needSpace){  //return the last process's point before the allowed space
	PCB* p = head;
	while (p != tail) {
		if (p -> next -> base - (p -> base + p -> limit) < needSpace)p = p->next;
		else return p;
	}
	return NULL;
}

PCB* bestFit (int needSpace){
	PCB* p = head;
	PCB* best = NULL;
	int minFitSpace = MAX;
	while (p != tail) {
		int nextSpace = (p -> next -> base - (p -> base + p -> limit));
		if (nextSpace >= needSpace && nextSpace < minFitSpace) {
			minFitSpace = nextSpace;
			best = p;
		}
		p = p->next;
	}
	return best;
}

PCB* worstFit (int needSpace){
	PCB* p = head;
	PCB* worst = NULL;
	int maxFitSpace = MIN;
	while (p != tail) {
		int nextSpace = (p -> next -> base - (p -> base + p -> limit));
		if (nextSpace >= needSpace && nextSpace > maxFitSpace) {
			maxFitSpace = nextSpace;
			worst = p;
		}
		p = p->next;
	}
	return worst;
}

void request (char* pname, int needSpace, char mode){
	PCB* lastp;
	switch (mode) {
		case 'F': lastp = firstFit (needSpace);  break;
		case 'B': lastp = bestFit (needSpace);  break;
		case 'W': lastp = worstFit (needSpace);  break;
		default: printf ("ERROR: INVALID MODE\n");
	}
	if (lastp == NULL) {
		printf ("NO ANY ENOUGH SPACE\n");
	}
	printf("SUCCESS: ALLOCATE SPACE FOR %s\n", pname);
	lastp -> next = newPCB (pname, lastp -> base + lastp -> limit, needSpace, lastp -> next);
	//tail can not be allocated to process
}

void release (char* pname) {
	PCB* p = head;
	while (p->next != tail) { //see the next process
		if (strcmp (pname, p -> next -> pname) == 0){
			PCB* tmp = p -> next;
			p -> next = tmp -> next;
			free(tmp);
			printf("SUCCESS: RELEASE %s\n", pname);
		}else p = p -> next;
	}
	printf("ERROR: NO PROCESS %s\n", pname);
}

void printStat(){
	PCB* p = head -> next; //first real process
	int lastEnd = 0;
	while (p != tail) {
		if (p -> base != lastEnd){
			printf ("Addresses [%d:%d] Unused\n", lastEnd, p -> base - 1);
		}
		printf ("Addresses [%d:%d] Process %s\n", p->base, p -> base + p->limit - 1, p -> pname);
		lastEnd = p -> base + p -> limit;
		p = p -> next;
	}
	if(lastEnd != tail -> base){
		printf ("Addresses [%d:%d] Unused\n", lastEnd, tail -> base - 1);
	}
}

void compact(){
	PCB* p = head;
	while (p -> next != tail){
		p -> next -> base = p -> base + p -> limit;
		p = p -> next;
	}
	printf("SUCCESS: UNUSED HOLES COMPACTED\n");
}

void destroy() {
	PCB* p = head;
	while (p != NULL) {
		PCB* tmp = p;
		p = p -> next;
		free (tmp);
	}
}

int main (int argc, char* argv[]){
	int memory_MAX;
	int needSpace;
	char cmd[CMD_LEN_MAX], pname[CMD_LEN_MAX], mode;
	if (strcmp(argv[1],"-h")==0){
		printf("command like ./allocator 1048576  (in bytes, eg. 1MB)\n");
		return 0;} 
	memory_MAX = atoi(argv[1]);

	tail = newPCB ("TAIL", memory_MAX, 0, NULL);
	head = newPCB ("HEAD", 0, 0, tail);
	
	while (1) {
		printf ("allocator> ");
		scanf ("%s", cmd);
		if (strcmp(cmd, "Q") == 0) break;
		else if (strcmp(cmd, "RL") == 0){
			scanf ("%s", pname);
			release(pname);
		}else if (strcmp(cmd, "RQ") == 0){
			scanf ("%s", pname);
			scanf ("%d", &needSpace);
			getchar();
			mode = getchar();
			request(pname, needSpace, mode);
		}else if (strcmp (cmd, "STAT") == 0)printStat();
		else if (strcmp (cmd, "C") == 0)compact();

		while ((getchar())!= '\n');//clear the buffer
	}
	destroy();
	return 0;
}