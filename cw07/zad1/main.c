#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <sys/mman.h>
#include<time.h>
#include "common.h"
#include <signal.h>
#include<sys/wait.h>
#define BARBER_EXE "/home/szymon/Pulpit/studia/sysopy/cw07/zad1/barber.exe"
void create_sem();
void spawn(int n);
void end_handler(int n);
void print_time();
// pamięć wspólna dla czasu fryzury
int sem;
int main(){
    create_sem();
    signal(SIGINT,end_handler);
    for(int i=0; i<BARBERS_SIZE;i++)
    {
        spawn(i);
    }
    while (wait(NULL)>0)
    {
    }
    
    end_handler(-1);
    return 0;
}
int check_clients(){

}
void print_time(){
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("%s",asctime(timeinfo));
}
void create_sem(){
    sem = semget(ftok("/",SEM_ID),4,IPC_CREAT|0666);
    unsigned short array[] = {BARBERS_SIZE,CLIENTS_SIZE,CHAIRS_SIZE,QUEUE_SIZE-CLIENTS_SIZE};
    union semun arg;
    arg.array = array;
    semctl(sem, -1, SETALL, arg); // second argument is ignored

    // printf("b %d c %d ch %d q %d\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL));
}
void spawn(int n){
    pid_t client = fork();
    char arg[3];
    sprintf(arg," %d",n);
    if(client == 0){
        execl(BARBER_EXE,BARBER_EXE,arg,NULL);
    }
}

void end_handler(int n){
    printf("Closing\n");
    semctl(sem, -1, IPC_RMID);
    exit(0);
}