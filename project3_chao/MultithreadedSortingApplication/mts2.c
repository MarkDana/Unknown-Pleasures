


/**
原来由于pthread库不是Linux系统默认的库，连接时需要使用库libpthread.a,所以在使用pthread_create创建线程时，在编译中要加-lpthread参数:
gcc -o test -lpthread test.c   这样好像不成  改成pthread才可以
再查发现编译时参数写成 -pthread 也是可以的。
————————————————
版权声明：本文为CSDN博主「FinnAwe」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/liangsir_l/article/details/50638739
**/

////////////////////////////////
// Amy Puente                 //
// CS 426 Assignment 2        //
// CH. 4 Project Solution     //
////////////////////////////////

/*
This program is a solution to the chapter 4 programming project 2 in the OS textbook. The program takes in a filename on the command line. The file should have
integer values, one per line, which the program reads into an int array.
The program then creates 2 threads, one to sort each half of the array.
Then the program creates a third thread which merges the 2 subarrays.
The parent thread then outputs the sorted master array.
To compile the program, use the command "gcc part1.c -pthread" 
To run the command, run the executable file with a data file name on the command line.
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#define Max 10000

int data_origin[Max];
int data_sorted[Max];
FILE *freader;
int num;
char file_name[100];





int cmp(const void *a, const void *b)
{
    return *(int*)a - *(int*)b; //由小到大排序  //return *(int *)b - *(int *)a; 由大到小排序
}

typedef struct{
int *head;
int start;
int num;
}parameters;

typedef struct{
int *array1;
int num1;
int *array2;
int num2;
int *res;
}merge_data;

void* sort_half(void *param){
    parameters *p=param;
    int *head=p->head;
    int start=p->start;
    int num=p->num;
    printf("我是负责排序从第%d到第%d位的子进程.\n",start,start+num-1);
    qsort(head,num,sizeof(int),cmp);
    printf("第%d到第%d位排序完毕.\n",start,start+num-1);
    pthread_exit(0);
}

void* merge(void *param){
    printf("我是负责归并的子进程.\n");
    merge_data *p=param;
    int *array1=p->array1;
    int *array2=p->array2;
    int *res=p->res;
    int num1=p->num1;
    int num2=p->num2;
    int i=0,j=0,k=0;
    while(i<num1&&j<num2){
        if(array1[i]<=array2[j]){
            res[k++]=array1[i++];
        }else res[k++]=array2[j++];
    }
    while(i<num1) res[k++]=array1[i++];
    while(j<num2) res[k++]=array2[j++];
    printf("归并完毕.\n");
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    char content[100];
    int res;
    pthread_t tid1,tid2,tid3;
    pthread_attr_t attr1,attr2,attr3;
    parameters *first=(parameters *) malloc(sizeof(parameters));
    parameters *second=(parameters *) malloc(sizeof(parameters));
    merge_data *third=(merge_data*) malloc(sizeof(merge_data));
    strcpy((char *)file_name, argv[1]);

    freader=fopen((char *)file_name,"r");
     while(fgets((char *)content,100,freader)!=NULL){
         data_origin[num++]=strtol((char *)content,NULL,10);
    }

    printf("数据读取完毕，一共%d个数据.\n",num);
    /*
    qsort(&data_origin[0],num,sizeof(int),cmp);
    for(int i=0;i<num;++i)
        printf("%d\n",data_origin[i]);
    */

    
    first->head=&(data_origin[0]);
    first->num=num/2;
    first->start=0;

    second->head=&(data_origin[num/2]);
    second->num=num-num/2;
    second->start=num/2;

    third->array1=&(data_origin[0]);
    third->num1=num/2;
    third->array2=&(data_origin[num/2]);
    third->num2=num-num/2;
    third->res=&(data_sorted[0]);


    
    pthread_attr_init(&attr1); 
    pthread_create(&tid1,&attr1,sort_half, first); 
    pthread_attr_init(&attr2); 
    pthread_create(&tid2,&attr2,sort_half, second); 
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_attr_init(&attr3); 
    pthread_create(&tid3,&attr3,merge, third); 
    pthread_join(tid3, NULL);

    printf("是否输入排序后的结果? 0 表示不输出 1表示输出\n");
    scanf("%d",&res);
    if(res!=0){
        printf("现把结果输出如下:\n");
        for(int i=0;i<num;++i)
            printf("%d\n",data_sorted[i]);
    }
    return 0;

    
//     pthread_exit(0);    
    
//     return 0;
    
}


