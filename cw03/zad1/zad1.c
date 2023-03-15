#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include"zad1.h"

void make_process(int i, int max){

    pid_t child_pid = fork();
    if(child_pid == 0){
        printf("PPID: %d, PID: %d\n",(int)getppid(),(int)getpid());
    }
    else if(i+1<max){
        make_process(i+1, max);
    }
    else{
        for(int i=0; i<max; i++) wait(NULL);
        printf("Number of processes: %d\n",max);
    }

}

int main( int argc, char*argv[]){
    if(argc != 2 || is_number(argv[1]) == 0){
        fprintf(stderr, "Wrong arguments\n");
        return -1;
    }
    int n = atoi(argv[1]);
    make_process(0,n);

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