#include<stdio.h>
#include <stdint.h>
#include<string.h>
#define TLB_SIZE 16
#define PAGES 256
#define FRAMES 128
#define PAGE_SIZE 256 // aka frame size
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define MEMORY_SIZE (FRAMES * PAGE_SIZE) // physical memory size

typedef struct tlb_entry{
	int logical_page;
	int physical_page;
}TLB_Entry;

TLB_Entry TLB[TLB_SIZE];
int page_table[PAGES]; //need to init to all -1
int frame2page[FRAMES]; // from frame id in memory to page id in page table
//above 2 is reverse
int8_t memory[MEMORY_SIZE]; // use int8_t as 8 bytes 

int TLB_index=0; 
int memory_index=0;

FILE *BACKING_STORE;
FILE *address_list;


int TLB_lookup(int logical_page){
    int i;
    for(i = 0; i < TLB_SIZE; ++i){
        if(TLB[i].logical_page == logical_page)
            return TLB[i].physical_page;}
    return -1;
}

void TLB_insert(int logical_page, int physical_page){  
    TLB[TLB_index].logical_page = logical_page; 
    TLB[TLB_index].physical_page = physical_page;
    ++TLB_index;
    TLB_index = TLB_index % TLB_SIZE;
}


int main(int argc, char** argv){
    int total = 0;
    int hits = 0;
    int faults = 0;

    for(int i = 0; i < PAGES; ++i)page_table[i] = -1;
    for(int i = 0; i < FRAMES; ++i)frame2page[i] = -1;
    for(int i = 0; i < TLB_SIZE; ++i)TLB[i].logical_page =-1;

    BACKING_STORE = fopen("BACKING_STORE.bin","r");
    address_list = fopen(argv[1],"r");

    int logical_address;
    while(fscanf(address_list, "%d", &logical_address) != EOF){
        ++total;
		int offset = logical_address & OFFSET_MASK;
		int logical_page = (logical_address >> OFFSET_BITS) & OFFSET_MASK;
		int physical_page = TLB_lookup(logical_page);
        if(physical_page != -1)++hits;
        else{
            physical_page = page_table[logical_page];
            if (physical_page == -1){ //page fault
                ++faults;
                if(frame2page[memory_index] != -1){ //need replacement (full, cover)
                    int replacement_page = frame2page[memory_index];
                    for(int i = 0;i < TLB_SIZE; ++i)if(TLB[i].logical_page == replacement_page)TLB[i].logical_page = -1;
                    page_table[replacement_page] = -1;
                }
                physical_page = memory_index;
                ++ memory_index;
                memory_index = memory_index % FRAMES;
                fseek(BACKING_STORE, logical_page * PAGE_SIZE, SEEK_SET); 
                // redirect BACKING_STORE, offset logical_page*PAGE_SIZE bytes
                fread(memory + physical_page * PAGE_SIZE, 1, PAGE_SIZE, BACKING_STORE);
                frame2page[physical_page] = logical_page; // the original memory_index
                page_table[logical_page] = physical_page;
            }
            TLB_insert(logical_page, physical_page);
        }

        int physical_address = (physical_page << OFFSET_BITS) | offset;
		int8_t value = memory[physical_address];
		printf("Virtual address: %d Physical address: %d Value: %d\n" ,logical_address ,physical_address ,value);
        
    }
    printf("\nTotal: %d\tPage Faults:%d\tPage Fault Rate:%.3f\tTLB Hits:%d\tTLB Hit Rate:%.3f\n",
    total, faults, faults / (1.0 * total), hits, hits / (1.0 * total));
    
    fclose(address_list);
    fclose(BACKING_STORE);
  
}