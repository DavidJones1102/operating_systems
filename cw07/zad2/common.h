#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#define BARBERS_SIZE 3
#define CLIENTS_SIZE 5
#define CHAIRS_SIZE 2
#define QUEUE_SIZE 10 

#define BARBERS 0
#define CLIENTS 1
#define CHAIRS 2
#define QUEUE 3
#define QUEUE_SYNC 4
#define QUEUE_LEN 5

#define BARBERS_NAME "/sem_barbers"
#define CLIENTS_NAME "/sem_clients"
#define CHAIRS_NAME "/sem_chairs"
#define QUEUE_NAME "/sem_queue"
#define QUEUE_SYNC_NAME "/sem_queue_sync"
#define QUEUE_LEN_NAME "/sem_queue_len"

#define SHARED_NAME "/shared_mem"
#define SEM_SIZE 6

void print_all(sem_t** sem){
    int b;
    int cl;
    int ch;
    int q;
    int qs;
    int ql;
    sem_getvalue(sem[BARBERS],&b);
    sem_getvalue(sem[CLIENTS],&cl);
    sem_getvalue(sem[CHAIRS],&ch);
    sem_getvalue(sem[QUEUE],&q);
    sem_getvalue(sem[QUEUE_SYNC],&qs);
    sem_getvalue(sem[QUEUE_LEN],&ql);

    printf("b%d cl%d ch%d q%d qs%d ql%d\n",b,cl,ch,q,qs,ql );
    fflush(stdout);
}
