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

typedef struct{
int *head;  //排序元素的起始地址
int start;  // 起始下标
int num;   // 需要排序的元素数目
}parameters;

typedef struct{
int *array1; // 待归并的第一个数组的起始地址
int num1; // 待归并的第一个数组的元素数目
int *array2; // 待归并的第二个数组的起始地址
int num2; // 待归并的第二个数组的元素数目
int *res; //存放结果的数组的起始地址
}merge_data;

int cmp(const void *a, const void *b) //qsort需要的比较函数
{
    return *(int*)a - *(int*)b; //由小到大排序  //return *(int *)b - *(int *)a; 由大到小排序
}

void* sort_half(void *param){
    parameters *p=param;
    int *head=p->head;
    int start=p->start;
    int num=p->num;
    printf("我是负责排序从第%d到第%d位的子进程.\n",start,start+num-1);
    qsort(head,num,sizeof(int),cmp);
    // https://blog.csdn.net/f_zyj/article/details/51484751 关于qsort的使用
    printf("第%d到第%d位排序完毕.\n",start,start+num-1);
    pthread_exit(0);
}

void* merge(void *param){ // 进行归并
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
    //要求把变量定义都写在前面 
    // 传参数的时候，不能传数组名，要&取地址之后再传，传进去的是普通指针，sizeof()显示的是指针的大小，
    // 而不是数组的大小，所以要有参数说明数组的大小。

    char content[100];   
    int res;
    pthread_t tid1,tid2,tid3;
    pthread_attr_t attr1,attr2,attr3;
    parameters *first=(parameters *) malloc(sizeof(parameters));
    parameters *second=(parameters *) malloc(sizeof(parameters));
    merge_data *third=(merge_data*) malloc(sizeof(merge_data));
    
    strcpy((char *)file_name, argv[1]); // 获取要读取数据的文件名
    freader=fopen((char *)file_name,"r"); //读取数据
     while(fgets((char *)content,100,freader)!=NULL){
         data_origin[num++]=strtol((char *)content,NULL,10);
    }

    printf("数据读取完毕，一共%d个数据.\n",num);
    
    //设置三个线程所需要参数的信息
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

    //创建两个sorting thread
    pthread_attr_init(&attr1); 
    pthread_create(&tid1,&attr1,sort_half, first); 
    pthread_attr_init(&attr2); 
    pthread_create(&tid2,&attr2,sort_half, second); 
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    //两个sotring thread执行完，创建merging thread
    pthread_attr_init(&attr3); 
    pthread_create(&tid3,&attr3,merge, third); 
    pthread_join(tid3, NULL);

    //提示是否输出结果
    printf("是否输入排序后的结果? 0 表示不输出 1表示输出\n");
    scanf("%d",&res);
    if(res!=0){
        printf("现把结果输出如下:\n");
        for(int i=0;i<num;++i)
            printf("%d\n",data_sorted[i]);
    }
    return 0;
   
}
