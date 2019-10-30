#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#define Max 10000

int data_origin[Max];
int data_sorted[Max];
FILE *freader;
int num;

typedef struct{
int *head;  // head of the array
int left;  // starting index
int right;   // ending index
}parameters;

typedef struct{
int *array1;
int num1;
int *array2;
int num2;
int *res;
}merge_data;

void quick_sort(int *a, int l, int r){
    printf("sorting from %d to %d\n",l,r);
    if (l < r){
        int i,j,x;
        i = l;
        j = r;
        x = a[i];
        while (i < j){
            while(i < j && a[j] > x)j--; // find the first num<x from right to left
            if(i < j)a[i++] = a[j];

            while(i < j && a[i] < x)i++; // find the first num>x from left to right
            if(i < j)a[j--] = a[i];
        }
        a[i] = x;
        quick_sort(a, l, i-1);
        quick_sort(a, i+1, r);
    }
}

void* sort_half(void *param){
    parameters *p=param;
    int *head=p->head;
    int left=p->left;
    int right=p->right;
    printf("thread sorting from index %d to %d.\n",left,right);
    quick_sort(head,left,right);
    for (int puu=left;puu<=right;++puu)printf("%d ",head[puu]);
    printf("\n");
    pthread_exit(0);
}

void* merge(void *param){ // 进行归并
    printf("thread merge two sorted arrays.\n");
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
    printf("merged.\n");
    pthread_exit(0);
}

int main(int argc, char *argv[]){  

    char content[100],file_name[100];;   
    int res;
    pthread_t tid1,tid2,tid3;
    pthread_attr_t attr1,attr2,attr3;
    parameters *first=(parameters *) malloc(sizeof(parameters));
    parameters *second=(parameters *) malloc(sizeof(parameters));
    merge_data *third=(merge_data*) malloc(sizeof(merge_data));
    
    strcpy(file_name, argv[1]);

    freader=fopen(file_name,"r");
     while(fgets((char *)content,100,freader)!=NULL){
         data_origin[num++]=strtol((char *)content,NULL,10);
    }

    printf("数据读取完毕，一共%d个数据.\n",num);
    
    //设置三个线程所需要参数的信息
    first->head=&(data_origin[0]);
    first->left=0;
    first->right=num/2-1;

    second->head=&(data_origin[0]);
    second->left=num/2;
    second->right=num-1;

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
