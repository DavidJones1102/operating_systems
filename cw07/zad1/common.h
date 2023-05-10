#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <signal.h>

#define BARBERS_SIZE 7
#define CLIENTS_SIZE 10
#define CHAIRS_SIZE 5
#define QUEUE_SIZE 10 

#define BARBERS 0
#define CLIENTS 1
#define CHAIRS 2
#define QUEUE 3
#define QUEUE_SYNC 4
#define QUEUE_LEN 5

#define SEM_ID 1
#define SHARED_ID 3
#define SEM_SIZE 6

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

void print_all(){
    int sem = semget(ftok("/",SEM_ID),SEM_SIZE,0);
    printf("b %d c %d ch %d q %d s %d l %d\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL),semctl(sem,4,GETVAL),semctl(sem,5,GETVAL));
    fflush(NULL);
}