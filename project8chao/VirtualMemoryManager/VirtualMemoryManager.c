#include<stdio.h>
#include <stdint.h> // 使用uint8_t int8_t
#include<string.h>
#define TLB_SIZE 16  //  TLB的大小
#define PAGES 256    //  一共有多少页
#define PAGE_SIZE 256 // 页的大小 同时也是帧的大小
#define OFFSET_BITS 8 // 后8位是page offset
#define OFFSET_MASK 255	 //和它与的结果 等同于对255取模
#define FRAMES 128 // 一共有多少帧 
#define MEMORY_SIZE (FRAMES * PAGE_SIZE) // 内存的大小有帧数和页的大小决定

enum bool{false,true}; // 定义的bool枚举类型 false:0  true:1 

typedef struct T{
	int logical_page;   // 逻辑页
	int physical_page;  // 物理页
    enum bool is_valid; // 标志位 true代表当前逻辑页和物理页的对应是合法的，有意义的
}TLB_Entry; // TLB中的元素

typedef struct K{
int physical_page; // 物理页
enum bool is_valid; // true代表当前逻辑页和物理页的对应是合法的，有意义的
}Page_Table_Entry; // 页表中的元素


TLB_Entry TLB[TLB_SIZE]; // TLB translation lookaside buffer
Page_Table_Entry page_table[PAGES]; // 页表
int which_page[FRAMES]; // 记录内存中当前块对应哪个逻辑页
int8_t memory[MEMORY_SIZE]; // 模拟内存 之所以用int8_t是因为它刚好是一个字节 8个bit 

// TLB和页面置换使用的都是FIFO 所以我们要记录替换/置换发生的位置，要注意对相应的size取模，起到循环的效果 
int TLB_index=0; 
int memory_index=0;


FILE *BACKING_STORE; // 指向BACKING_STORE.bin文件的文件流
FILE *address_list; //指向addresses.txt文件的文件流

// 初始化 就是设置TLB page_table which_page, 打开BACKING_STORE.bin
void init(){
    int i;
    for(i=0;i<TLB_SIZE;++i)
        TLB[i].is_valid=false; //初始的时候，所有的对应关系都是不正确的
    for(i=0;i<PAGES;++i)
        page_table[i].is_valid=false; //初始的时候，所有的对应关系都是不正确的
    for(i=0;i<FRAMES;++i)
        which_page[i]=-1; // -1代表这个内存块没有指向任何逻辑页
    BACKING_STORE=fopen("BACKING_STORE.bin","r");    //打开文件 r:只读
}

// 在TLB中进行查找 
int TLB_lookup(int logical_page){
    int i;
    for(i=0;i<TLB_SIZE;++i){
        if(TLB[i].is_valid==true && TLB[i].logical_page==logical_page) //如果对应关系是正确的，而且记录的正好是要查找逻辑页
            return TLB[i].physical_page; //返回物理页
    }
    return -1; //返回-1代表没找到
}

// 在TLB中进行插入/替换
void TLB_insert(int logical_page,int physical_page){  
    // TLB_index记录的就是当前的插入/替换位置，依次设置逻辑页,物理页,标志位
    TLB[TLB_index].logical_page=logical_page; 
    TLB[TLB_index].physical_page=physical_page;
    TLB[TLB_index].is_valid=true;
    ++TLB_index; //更新TLB_index 
    TLB_index=TLB_index%TLB_SIZE; //要对TLB的大小进行取模,实现循环
 
}


int main(int argc, char** argv){
    int logical_address; //逻辑地址
    int logical_page; //逻辑页
    int physical_address; //物理地址
    int physical_page; //物理页
    int total=0; //记录一共有多少次地址访问
    int TLB_hits=0; //记录TLB命中的次数
    int page_faults=0; //记录page fault的次数
    int offset; //业内的偏移量 0-255
    int replacement_page; //被置换的逻辑页
    int i,k;
    int value; //被访问字节的值
    char filename[1000];
    char addresses[]="addresses.txt";
    init(); //进行初始化
    if(argc<2){ // 如果没输入参数 那就默认是当前addresses.txt文件夹
        strcpy(filename,argv[0]);
        for(i=sizeof(filename)-1;i>=0;--i) //获取当前目录
            if(filename[i]=='/')
                break;
        ++i; 
        for(k=0;k<sizeof(addresses);++k)
            filename[i+k]=addresses[k];
        // printf("%s\n",filename);
        address_list=fopen(filename,"r"); // 打开文件
    } 
    else
        address_list=fopen(argv[1],"r"); // 打开文件
   
    while(fscanf(address_list, "%d", &logical_address) != EOF ){ //获取逻辑地址
        ++total; //总数增加
		offset = logical_address & OFFSET_MASK; //获得页内偏移量
		logical_page = (logical_address >> OFFSET_BITS) & OFFSET_MASK; //获取逻辑页
        // printf("%d\n",logical_page);
		physical_page = TLB_lookup(logical_page); //在TLB中查找
        // printf("%d\n",physical_page);
        if(physical_page!=-1){ //在TLB中找到了
            ++TLB_hits;
            // printf("hit\n");
        }else{
            // printf("miss\n");
            if(page_table[logical_page].is_valid==false){ //page fault
                // printf("page fault\n");
                ++page_faults;
                if(which_page[memory_index]!=-1) {  //需要页面置换 因为当前的内存块储存着其他的逻辑页
                    // printf("page replacement\n");
                    replacement_page=which_page[memory_index]; //被替换的逻辑页
                    for(i=0;i<TLB_SIZE;++i) //如果该逻辑页在TLB中 将之设为无效
                        if(TLB[i].logical_page==replacement_page)
                            TLB[i].is_valid=false;
                    page_table[replacement_page].is_valid=false; //同理在page_table中也设为无效
                }
                physical_page=memory_index; //获取物理页
                ++memory_index; // 内存中插入/替换位置更新
                memory_index=memory_index%FRAMES;// 同样要对帧数进行取模 实现循环
                fseek(BACKING_STORE,logical_page*PAGE_SIZE,SEEK_SET); 
                // 将BACKING_STORE重定向，从SEEK_SET(文件的开头)向后偏移logical_page*PAGE_SIZE字节
                fread(memory+physical_page*PAGE_SIZE,1,PAGE_SIZE,BACKING_STORE);
                // memory+physical_page*PAGE_SIZE代表将读取到的内容从这里开始往后存，1代表我们每次读取1字节，
                // PAGE_SIZE代表要读取PAGE_SIZE多次,BACKING_STORE是文件流
                which_page[physical_page]=logical_page; // 当前内存块记录的是当前逻辑页的内容
                page_table[logical_page].physical_page=physical_page; // 更新页表中的记录信息
                page_table[logical_page].is_valid=true;  //设置标志位
                
            }else {
                physical_page=page_table[logical_page].physical_page;
            }
            TLB_insert(logical_page,physical_page); //由于TLB miss 所以要把当前的逻辑页和物理页对应关系添加进TLB中
        }
        physical_address = (physical_page << OFFSET_BITS) | offset; //获取物理地址
		value = memory[physical_address]; //获取值
		printf("Virtual address: %d Physical address: %d Value: %d\n" ,logical_address ,physical_address ,value); //输出相关信息
        
    }
    printf(	"total:[%d]  page-fault:[%d]  page-fault rate:[%.3f]  TLB hits:[%d]  TLB hit rate:[%.3f]\n",
    total, page_faults, page_faults / (1.0 * total), TLB_hits, TLB_hits / (1.0 * total));   //输出统计信息
    
    fclose(address_list); //关闭文件
    fclose(BACKING_STORE);
  
}