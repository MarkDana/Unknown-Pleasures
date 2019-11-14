#include<iostream>
#include<cstdio>
#include<string.h>
using namespace std;
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];
/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request[NUMBER_OF_RESOURCES]; //请求资源数组
int release[NUMBER_OF_RESOURCES]; //释放资源数组
int request_resources(int customer, int request[]);
void release_resources(int customer, int release[]); //执行资源的回收
bool safe(int customer, int request[]); //sage函数判断当前系统是否处于安全状态
void give(int customer, int request[]); //give函数执行资源的分配
void output(); //输出各种信息
void init(); //读取输入 进行初始化


int main() {
	int customer,i;
	char command[10];
	init();
	while (1) {
		cout<<endl<<"请输入命令,RQ 0 3 1 2 1 代表0号进程申请4类资源数分别为3,1,2,1; RL 4 1 2 3 1代表4号进程释放4类资源数分别为1,2,3,1;*代表输出当前各种值;EXIT表示退出"<<endl;
		
		cin>>command;
		if( strcmp(command,"RQ")==0){ //RQ 则获取是哪个进程请求资源，请求多少资源
			cin>>customer;
			for(i=0;i<NUMBER_OF_RESOURCES;++i)
				cin>>request[i];
			if(request_resources(customer,request)==0){ //资源请求成功
				printf("%d号进程请求资源成功!\n",customer);
			}else printf("%d号进程请求资源失败!\n",customer);
		}

		if( strcmp(command,"RL")==0){ //RL 获取是哪个进程释放资源，释放多少资源
			cin>>customer;
			for(i=0;i<NUMBER_OF_RESOURCES;++i)
				cin>>release[i];
			release_resources(customer,release);
			printf("%d号进程已拥有合理资源释放完成!\n",customer);
		}
		if( strcmp(command,"*")==0){ //输出各种信息
			output();
		}
		if( strcmp(command,"EXIT")==0) //退出
			break;
	
	}
}

void give(int customer, int request[]){ //执行资源的分配 把customer请求的资源分配给它
	for(int i=0;i<NUMBER_OF_RESOURCES;++i){
		available[i]-=request[i];
		need[customer][i]-=request[i];
		allocation[customer][i]+=request[i];
	}
}

int request_resources(int customer, int request[]){
	bool flag;
	for(int i=0;i<NUMBER_OF_RESOURCES;++i){
		if(request[i]>need[customer][i]){ //该进程索要的资源数加上获得的资源数已经超过了它声明的它需要的最大资源数 不予分配
			printf("%d号进程索要了超过其声明的资源，请求资源失败!\n",customer);
			return -1;
		}
		if(request[i]>available[i]){ //该进程索要的资源超过了系统目前可分配的该资源的数量 不予分配
			printf("目前系统没有足够的资源，%d号进程请求资源失败!\n",customer);
			return -1;
		}	
	}
	give(customer, request); //先尝试把该资源分配给这个进程，看看系统能不能处于安全状态
	flag=safe(customer,request);
	if(flag) return 0; //系统处于安全状态 
	else{ //系统处于不安全的状态 需要把分配给这个进程的资源再收回
		release_resources(customer,request); //收回资源
		printf("分配后系统处于不安全状态，%d号进程请求资源失败!\n",customer);
		return -1;
	}
}

bool safe(int customer, int request[])
{	
	int work[NUMBER_OF_RESOURCES]; //记录目前系统可用的资源
	bool finish[NUMBER_OF_CUSTOMERS]={0}; //记录某个进程是否可以完成
	for(int i=0;i<NUMBER_OF_RESOURCES;++i)
		work[i]=available[i];
	int num=0;
	while(num<NUMBER_OF_CUSTOMERS){ //如果num>=NUMBER_OF_CUSTOMERS 说明要么finish的值都为true 要么当前的资源不能让任何一个进程执行完毕
		if(finish[num]==true){ //进程可以完成 则跳过 看下一进程
			++num;
			continue;
		}
		bool flag=true;
		for(int i=0;i<NUMBER_OF_RESOURCES;++i)
			if(need[num][i]>work[i]){ //需要的资源数超过了目前系统可以提供的资源数 跳出循环
				flag=false; 
				break;
			}
		if(flag){ //说明系统资源可以供该进程执行完成
			for(int i=0;i<NUMBER_OF_RESOURCES;++i)
				work[i]+=need[num][i]; //释放资源
			finish[num]=true;//该进程已完成
			num=0;//再从头开始扫描
			continue;
		}else{
			++num; //该进程不能完成，尝试下一进程
			continue;
		}
	}
	for(int i=0;i<NUMBER_OF_CUSTOMERS;++i)
		if(!finish[i])  //有某一进程没有完成
			return false; //返回false
	return true;
}

void release_resources(int customer, int release[]){ //释放资源 但是要防止某一进程释放的资源数大于它实际被分配的资源数
	int realistic;
	for(int i=0;i<NUMBER_OF_RESOURCES;++i){
		realistic=min(release[i],allocation[customer][i]); //取要释放的资源数和实际拥有的资源数的较小值
		available[i]+=realistic;
		need[customer][i]+=realistic;
		allocation[customer][i]-=realistic;
	}
	
}

void output(){ //输出各种信息
	int i,j;
	printf("现输出available情况:\n");
	for(i=0;i<NUMBER_OF_RESOURCES;++i)
		printf("%d\t",available[i]);
	printf("\n\n");

	printf("现输出maximum情况:\n");
	for(i=0;i<NUMBER_OF_CUSTOMERS;++i){
		for(j=0;j<NUMBER_OF_RESOURCES;++j)
			printf("%d\t",maximum[i][j]);
		printf("\n");
	}
	printf("\n");

	printf("现输出allocation情况:\n");
	for(i=0;i<NUMBER_OF_CUSTOMERS;++i){
		for(j=0;j<NUMBER_OF_RESOURCES;++j)
			printf("%d\t",allocation[i][j]);
		printf("\n");
	}
	printf("\n");

	printf("现输出need情况:\n");
	for(i=0;i<NUMBER_OF_CUSTOMERS;++i){
		for(j=0;j<NUMBER_OF_RESOURCES;++j)
			printf("%d\t",need[i][j]);
		printf("\n");
	}
	printf("\n输出完毕!\n");

	
}
void init() {
    int i,j;
	FILE *freader;
	printf("请输入4类资源初始资源数(10 5 7 8):\n"); //获取资源初始值
	for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
		scanf("%d",&available[i]);
	
	printf("现在从data.txt文件中获取5个进程对4类的最大使用数,请稍等\n"); //从data.txt中获取进程对每一资源的最大使用量
	freader=fopen("./data.txt","r"); 
	for(i = 0 ; i < NUMBER_OF_CUSTOMERS; ++i){
		for( j = 0; j <  NUMBER_OF_RESOURCES ; ++j){
			fscanf(freader,"%d",&maximum[i][j]);
			need[i][j]=maximum[i][j];
			allocation[i][j] = 0;
		}
	}
	printf("读取完毕，现将available,maximum,allocation和need情况依次输出:\n");
	output();
}