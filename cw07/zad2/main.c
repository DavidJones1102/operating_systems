#include "common.h"
#include<sys/wait.h>
#define BARBER_EXE "/home/szymon/Pulpit/studia/sysopy/cw07/zad2/barber.exe"
#define CLIENT_EXE "/home/szymon/Pulpit/studia/sysopy/cw07/zad2/client.exe"
void create_sem();
void create_shared_mem();
void spawn(int n, char* exe);
void end_handler(int n);
void print_time();

sem_t** sem;
char* shared_mem;
int shared_fd;
pid_t workers[BARBERS_SIZE];

int main(){
    sem = malloc(SEM_SIZE*sizeof(sem_t *));
    create_sem();
    create_shared_mem(); 
    signal(SIGINT,end_handler);

    for(int i=0; i<BARBERS_SIZE;i++)
    {
        spawn(i,BARBER_EXE);
    }
    
    for(int i=0; i<CLIENTS_SIZE ;i++)
    {
        spawn(i,CLIENT_EXE);
    }
    while (1)
    {
        sleep(1);
    }
    return 0;
}

void create_sem(){
    sem[BARBERS] = sem_open(BARBERS_NAME, O_CREAT|O_EXCL, 0666, 0);
    sem[CLIENTS] = sem_open(CLIENTS_NAME, O_CREAT|O_EXCL, 0666, 0);
    sem[CHAIRS] = sem_open(CHAIRS_NAME, O_CREAT|O_EXCL, 0666, CHAIRS_SIZE);
    sem[QUEUE] = sem_open(QUEUE_NAME, O_CREAT|O_EXCL, 0666, QUEUE_SIZE);
    sem[QUEUE_SYNC] = sem_open(QUEUE_SYNC_NAME, O_CREAT|O_EXCL, 0666, 1);
    sem[QUEUE_LEN] = sem_open(QUEUE_LEN_NAME, O_CREAT|O_EXCL, 0666, 0);
}
void spawn(int n, char* exe){
    pid_t child = fork();
    char arg[3];
    sprintf(arg," %d",n);
    if(child == 0){
        execl(exe,exe,arg,NULL);
    }
    else{
        workers[n] = child;
    }
}

void end_handler(int n){
    printf("Closing\n");
    for(int i; i<BARBERS_SIZE;i++){
        kill(SIGINT, workers[i]);
    }
    for(int i = 0;i<SEM_SIZE;i++){
        if(sem_close(sem[i]) == -1){
            perror("[main] sem_close\n");
        }
    }
    free(sem);
    int cnt = 0;
    cnt += sem_unlink(BARBERS_NAME);
    cnt += sem_unlink(CLIENTS_NAME);
    cnt += sem_unlink(CHAIRS_NAME);
    cnt += sem_unlink(QUEUE_NAME);
    cnt += sem_unlink(QUEUE_SYNC_NAME);
    cnt += sem_unlink(QUEUE_LEN_NAME);
    if(cnt<0){
        perror("[mian] unlink error\n");
    }
    printf("%s\n",shared_mem);
    
    if(munmap(shared_mem,2048) == -1){
        perror("[main] munmap\n");
    }
    if(shm_unlink(SHARED_NAME) == -1){
        perror("[main] munmap\n");
    }

    while (wait(NULL)>0)
    {}
    exit(0);
}


void create_shared_mem(){
    shared_fd = shm_open(SHARED_NAME, O_CREAT|O_RDWR ,0666);
    if(shared_fd == -1){
        perror("shm_open error \n");
        raise(SIGKILL);
    }

    if(ftruncate(shared_fd, 2048) == -1){
        perror("frtuncate error \n");
        fprintf(stderr, "Value of errno: %s\n", strerror(errno));
        raise(SIGKILL);
    }
    shared_mem = mmap(NULL, 2048, PROT_READ|PROT_WRITE, MAP_SHARED, shared_fd, 0);
    strcpy(shared_mem, "");
}