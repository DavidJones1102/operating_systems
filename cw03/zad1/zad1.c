#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include"zad1.h"

int main( int argc, char*argv[]){
    if(argc != 2 || is_number(argv[1]) == 0){
        fprintf(stderr, "Wrong arguments\n");
        return -1;
    }
    int n = atoi(argv[1]);

    pid_t* arr = malloc(sizeof(pid_t)*n);
    pid_t pid = getpid();
    for(int i = 0; i<n;i++){
        if(pid == getpid()){
            arr[i] = fork();
        }

        if(pid != getpid()){
            printf("PPID: %d, PID: %d\n",(int)getppid(),(int)getpid());
            break;
        }
    }

    if(pid == getpid()){
        for(int i=0; i<n; i++) waitpid(arr[i],NULL,0);
        printf("Number of processes: %d\n",n);
    }
    return 0;
}

int is_number(char* str){
    int length = strlen(str);
    for (int i=0;i<length; i++)
        if (!isdigit(str[i]))
        {
            return 0;
        }
    return 1;
}