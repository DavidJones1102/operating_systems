#include "common.h"
void open_sem();
void spawn_client(int sem);
void attach_mem();
void dettach_mem();
char* gen_haircut();
int client_nr;
char *shared_mem;
char hair[10];

int main(int arg, char* argv[]){
    srand(time(NULL)*getpid());

    int sem = semget(ftok("/",SEM_ID),SEM_SIZE,0);

    strcpy(hair, gen_haircut());
    attach_mem();
    client_nr = atoi(argv[1]);
    printf("Client %d chcę zrobić fryzure %s\n", client_nr, hair);
    fflush(stdout);
    spawn_client(sem);
    dettach_mem();

    return 0;
}

void spawn_client(int sem){
    if( semctl(sem, QUEUE ,GETVAL )==0){
        printf("Client %d Nie ma dla mnie miejsca w kolejce :'( \n", client_nr);
        fflush(stdout);
        exit(0);      
    }
    struct sembuf sops[3];//!!
    sops[0].sem_num = CLIENTS;
    sops[0].sem_op = 1;
    sops[0].sem_flg = IPC_NOWAIT;

    sops[1].sem_num = QUEUE;
    sops[1].sem_op = -1;
    sops[1].sem_flg = IPC_NOWAIT;

    sops[2].sem_num = QUEUE_SYNC;
    sops[2].sem_op = -1;
    sops[2].sem_flg = 0;
    if( semop(sem,sops,3) == -1 ){
        printf("semop error in client\n");
        exit(0);
    }

    strcat(shared_mem, hair);
    struct sembuf sops2[2];
    sops2[0].sem_num = QUEUE_SYNC;
    sops2[0].sem_op = 1;
    sops2[0].sem_flg = 0;

    sops2[1].sem_num = QUEUE_LEN;
    sops2[1].sem_op = 1;
    sops2[1].sem_flg = 0;
    if( semop(sem,sops2,2) == -1 ){
        printf("semop error in client1\n");
        exit(0);
    }
}

void attach_mem(){
    key_t key = ftok("/",SHARED_ID);
    int shared_id = shmget(key, 2048, 0);
    if(shared_id == -1){
        perror("shmget \n");
        fflush(stdout);
        exit(-1);
    }
    shared_mem = shmat(shared_id, 0, 0);
    if(shared_mem == NULL){
        perror("shmat \n");
        fflush(stdout);
        exit(-1);
    }
}
void dettach_mem(){
    if(shmdt(shared_mem) == -1){
        perror("Detach error!\n");
        fflush(stdout);
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
