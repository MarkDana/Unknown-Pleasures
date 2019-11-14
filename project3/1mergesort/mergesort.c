#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#define Max 10000

int data_origin[Max];
int data_sorted[Max];
FILE *freader;
int num;
char onenum[100],file_name[100]; 
int res;
pthread_t tid_l,tid_r,tid_m;
pthread_attr_t attr_l,attr_r,attr_m;

typedef struct{
int *head;  // head of the array
int left;  // starting index
int right;   // ending index
}myarray;

typedef struct{
int *array1;
int num1;
int *array2;
int num2;
int *res;
}mymerge;

void quick_sort(int *a, int l, int r){
    // printf("sorting from %d to %d\n",l,r);
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

void* sort_half(void *arr){
    //arr is a sruct myarray, for passing parameters
    myarray *p=arr;
    int *head=p->head;
    int left=p->left;
    int right=p->right;
    printf("thread sorting from index %d to %d.\n",left,right);
    quick_sort(head,left,right);
    for (int puu=left;puu<=right;++puu)printf("%d ",head[puu]);
    printf("\n");
    pthread_exit(0);
}

void* merge(void *meg){
    printf("thread merge two sorted arrays.\n");
    mymerge *p=meg;
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

    myarray *array_l=(myarray *) malloc(sizeof(myarray));
    myarray *array_r=(myarray *) malloc(sizeof(myarray));
    mymerge *merge_lr=(mymerge*) malloc(sizeof(mymerge));
    
    if (!strcmp(argv[1],"-i")){
        strcpy(file_name, argv[2]);
        freader=fopen(file_name,"r");
        while(fgets(onenum,100,freader)!=NULL)data_origin[num++]=strtol(onenum,NULL,10);
    }else{
        for (int argi=1;argi<argc;++argi){
            strcpy(onenum, argv[argi]);
            data_origin[num++]=strtol(onenum,NULL,10);
        }
    }
    
    printf("%d ints loaded.\n",num);
    
    array_l->head=&(data_origin[0]);
    array_l->left=0;
    array_l->right=num/2-1;

    array_r->head=&(data_origin[0]);
    array_r->left=num/2;
    array_r->right=num-1;

    merge_lr->array1=&(data_origin[0]);
    merge_lr->num1=num/2;
    merge_lr->array2=&(data_origin[num/2]);
    merge_lr->num2=num-num/2;
    merge_lr->res=&(data_sorted[0]);

    pthread_attr_init(&attr_l); 
    pthread_create(&tid_l,&attr_l,sort_half, array_l);
    pthread_attr_init(&attr_r); 
    pthread_create(&tid_r,&attr_r,sort_half, array_r);
    pthread_join(tid_l, NULL);
    pthread_join(tid_r, NULL);

    pthread_attr_init(&attr_m); 
    pthread_create(&tid_m,&attr_m,merge, merge_lr); 
    pthread_join(tid_m, NULL);

    printf("sorted result: ");
    for(int i=0;i<num;++i)printf("%d ",data_sorted[i]);
    printf("\n");
    return 0;
   
}
