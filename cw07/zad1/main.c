#include "common.h"
#include<sys/wait.h>
#define BARBER_EXE "/home/szymon/Pulpit/studia/sysopy/cw07/zad1/barber.exe"
#define CLIENT_EXE "/home/szymon/Pulpit/studia/sysopy/cw07/zad1/client.exe"
void create_sem();
void create_shared_mem();
void spawn(int n, char* exe);
void end_handler(int n);
void print_time();

int sem;
int shared_id;
char* shared_mem;
pid_t workers[BARBERS_SIZE];

int main(){
    create_sem();
    create_shared_mem();
    signal(SIGINT,end_handler);

    for(int i=0; i<BARBERS_SIZE;i++)
    {
        spawn(i,BARBER_EXE);
    }
    // printf("b %d c %d ch %d q %d s %d\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL),semctl(sem,4,GETVAL));
    for(int i=0; i<CLIENTS_SIZE ;i++)
    {
        spawn(i,CLIENT_EXE);
    }
    // printf("b %d c %d ch %d q %d s %d\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL),semctl(sem,4,GETVAL));
    while (1)
    {
    }
    
    // end_handler(-1);
    return 0;
}

void print_time(){
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("%s",asctime(timeinfo));
}
void create_sem(){
    sem = semget(ftok("/",SEM_ID),5,IPC_CREAT|0666);
    unsigned short array[] = {0,0,CHAIRS_SIZE,QUEUE_SIZE,1};
    union semun arg;
    arg.array = array;
    semctl(sem, -1, SETALL, arg); // second argument is ignored

    // printf("b %d c %d ch %d q %d\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL));
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
    semctl(sem, -1, IPC_RMID);
    printf(" %s\n",shared_mem);
    if(shmdt(shared_mem) == -1){
        perror("shmdt error \n");
        raise(SIGKILL);
    };
    for(int i; i<BARBERS_SIZE;i++){
        kill(SIGINT, workers[i]);
    }
    shmctl(shared_id, IPC_RMID, NULL);
    while (wait(NULL)>0)
    {}
    exit(0);
}


void create_shared_mem(){
    key_t key = ftok("/",SHARED_ID);
    shared_id = shmget(key, 2048, IPC_CREAT | 0666);
    if(shared_id == -1){
        perror("shmget error \n");
        raise(SIGKILL);
    }
    shared_mem = shmat(shared_id, 0, 0);
    if(shmat == NULL){
        perror("shmat error \n");
        raise(SIGKILL);
    }
    strcpy(shared_mem, "");
}