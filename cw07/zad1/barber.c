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
void open_sem();
void start_service(int sem);
void end_service(int sem);

int main(int arg, char* argv[]){
    int sem = semget(ftok("/",SEM_ID),SEM_SIZE,0);
    printf("Fryzjer %d pojawił się w pracy\n", atoi(argv[1]));

    start_service(sem);
    printf("Fryzjer %d\n", atoi(argv[1])); //jakoś przekazać numer fryzury
    sleep(10);
    end_service(sem);
    
    return 0;
}
void start_service(int sem){
    struct sembuf sops[4];
    sops[0].sem_num = BARBERS;
    sops[0].sem_op = -1;
    sops[1].sem_num = CLIENTS;
    sops[1].sem_op = -1;
    sops[2].sem_num = CHAIRS;
    sops[2].sem_op = -1;
    sops[3].sem_num = QUEUE;
    sops[3].sem_op = 1;
    semop(sem,sops,4);
}
void end_service(int sem){
    struct sembuf sops[2];
    sops[0].sem_num = BARBERS;
    sops[0].sem_op = 1;
    sops[1].sem_num = CHAIRS;
    sops[1].sem_op = 1;
    semop(sem,sops,2);
    printf("b %d c %d ch %d q %d\n\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL));
}
void open_sem(){
    // sem = 
    // printf("%d %d %d %d\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL));

}