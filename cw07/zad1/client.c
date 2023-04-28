#include "common.h"
void open_sem();
void spawn_client(int sem);
void attach_mem();
void dettach_mem();
char* gen_haircut();
void add_haircut_to_shared_mem(int sem, char* hair);
int client_nr;
char *shared_mem;

int main(int arg, char* argv[]){
    srand(time(NULL)*getpid());

    int sem = semget(ftok("/",SEM_ID),SEM_SIZE,0);
    char hair[10];
    strcpy(hair, gen_haircut());
    client_nr = atoi(argv[1]);
    printf("Client %d chcę zrobić fryzure %s\n", client_nr, hair);
    fflush(stdout);
    spawn_client(sem);
    attach_mem();
    add_haircut_to_shared_mem(sem, hair);
    // printf("\n%s \n",shared_mem);
    dettach_mem();
    // printf("Dzięki\n");
    return 0;
}

void spawn_client(int sem){
    if( semctl(sem, QUEUE ,GETVAL )==0){
        printf("Client %d Nie ma dla mnie miejsca w kolejce :'( \n", client_nr);
        exit(0);      
    }
    struct sembuf sops[2];
    sops[0].sem_num = CLIENTS;
    sops[0].sem_op = 1;
    sops[0].sem_flg = IPC_NOWAIT;

    sops[1].sem_num = QUEUE;
    sops[1].sem_op = -1;
    sops[1].sem_flg = IPC_NOWAIT;
    // printf("b %d c %d ch %d q %d\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL));
    if( semop(sem,sops,2) == -1 ){
        printf("semop error in client\n");
        exit(0);
    }
}
void open_sem(){
    // sem = 
    // printf("%d %d %d %d\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL));

}
void attach_mem(){
    key_t key = ftok("/",SHARED_ID);
    int shared_id = shmget(key, 2048, 0);
    shared_mem = shmat(shared_id, 0, 0);
}
void dettach_mem(){
    if(shmdt(shared_mem) == -1){
        perror("Detach error!\n");
        // exit(-1);
    };
}
char *gen_haircut(){
    switch (rand()%4)
    {
    case 0:
        return "buzzcut ";
        break;
    
    case 1:
        return "mulet ";
        break;

    case 2:
        return "afro ";
        break;

    case 3:
        return "na_łyso ";
        break;
    }
}
void add_haircut_to_shared_mem(int sem, char* hair){
    struct sembuf sops;
    sops.sem_num = QUEUE_SYNC;
    sops.sem_flg = 0 ;
    sops.sem_op = -1;
    semop(sem, &sops,1);
    strcat(shared_mem, hair);
    sops.sem_op = 1;
    semop(sem, &sops,1);   
}