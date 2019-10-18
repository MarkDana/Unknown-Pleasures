#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LINE 80//80 chars per line, per command
#define simple_cmd 0
#define in_redirect 1
#define out_redirect 2
#define ipc_pipe 3

char *args[MAX_LINE/2 + 1];//command line (of 80) has max of 40 arguments
int cmd_type,cmd_index;//id and index
int should_run = 1;
int arg_length, concurrent;
char command[MAX_LINE]={'\0'}, last_command[MAX_LINE]={'\0'};
char tmp[MAX_LINE]={'\0'};

void parse(char *command,char **args);//set complex_cmd if <>|
void execute(char **args);

int main(void){
    while (should_run){
        memset(command, 0, sizeof(command));//clear last command
        cmd_type=simple_cmd;cmd_index=-1;
        printf("osh>");
        fflush(stdout);
        int i=0;
        char c;
        
        while(i<MAX_LINE && (c=getchar())!='\n')command[i++]=c;
        if(i==MAX_LINE){
            fprintf(stderr, "Failed: command too long\n");
            while ((getchar())!= '\n');//clear the buffer
            continue;
        }
        
        if(!strcmp(command, "")){continue;}
        if(!strcmp(command, "exit")){should_run=0;continue;}
        if (arg_length==1&&!strcmp(command, "!!")){
            if (strcmp(last_command,"")){//not empty
                strcpy(command, last_command);
                printf("Last command: %s\n", command);
            }else{
                fprintf(stderr,"Failed: no commands in history.\n");
                continue;
            }
        }
        strcpy(last_command, command);
        
        strcpy(tmp,command);//since strtok will change input
        parse(tmp,args);
        

        
        if (!strcmp(args[arg_length-1],"&")){
            args[--arg_length] = NULL;
            concurrent = 1;
        }
        else concurrent= 0;
         
        pid_t pid=fork();
        
        if (pid < 0) {
            fprintf(stderr, "Failed: unable to fork\n");
            continue;
        }else if (pid==0){//child process (real cmd)
            execute(args);
            return 1;
        }else if (!concurrent)waitpid(pid,NULL,0);
        //parent process (idle/init)
    }
    return 0;
}

void parse(char *command,char **args){
    arg_length=0;
    char *p;
    p = strtok(command, " ");
    while (p){
        if (!strcmp(p,">")){
            cmd_type=out_redirect;
            cmd_index=arg_length;}
        else if (!strcmp(p,"<")){
            cmd_type=in_redirect;
            cmd_index=arg_length;}
        else if (!strcmp(p,"|")){
            cmd_type=ipc_pipe;
            cmd_index=arg_length;}
        args[arg_length++] = p;
        p = strtok(NULL, " ");}
    args[arg_length] = NULL;
}
void execute(char **args){
    switch (cmd_type) {
        case simple_cmd:
            execvp(args[0], args);
            break;
        case in_redirect:
            if (args[cmd_index+1]){
                int fd = open(args[cmd_index+1], O_RDONLY);
                dup2(fd, STDIN_FILENO);
                args[cmd_index]=NULL;
                execvp(args[0], args);
            }else{
                fprintf(stderr, "Failed: invalid <");
                return;
            }
            break;
        case out_redirect:
            if (args[cmd_index+1]){
                int fd = open(args[cmd_index+1], O_WRONLY | O_CREAT | O_TRUNC);
                dup2(fd, STDOUT_FILENO);
                args[cmd_index]=NULL;
                execvp(args[0], args);
            }else{
                fprintf(stderr, "Failed: invalid >");
                return;
            }
            break;
        case ipc_pipe:
            args[cmd_index] = NULL;
            int pipefd[2];
            pid_t pid;
            if (pipe(pipefd) == -1) {
                fprintf(stderr,"Failed: unable to build the pipe");
                return;
            }
            pid=fork();//codes following will be run by both processes
            if (pid<0){
                fprintf(stderr, "Failed: unable to fork\n");
                return;
            }else if (pid == 0) {
                close(pipefd[0]);//close read port of child process
                dup2(pipefd[1], STDOUT_FILENO);
                execvp(args[0],args);
            }else{//parent process
                waitpid(pid,NULL,0);
                close(pipefd[1]);//close write port
                dup2(pipefd[0], STDIN_FILENO);
                execvp(args[cmd_index+1],args+cmd_index+1);
            }
        default:
            break;
    }
    //execvp will terminate the current process and exit, if successful
    fprintf(stderr,"Failed: invalid command %s.\n",command);
}



    


