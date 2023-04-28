#include "common.h"

void start_service(int sem);
void end_service(int sem);
void spawn_barber(int sem);
void attach_mem();
void dettach_mem();
void tok(char* line);
void handler(int n);

char *shared_mem;
char hair[10];

int main(int arg, char* argv[]){
    int sem = semget(ftok("/",SEM_ID),SEM_SIZE,0);
    attach_mem();
    spawn_barber(sem);
    printf("Fryzjer %d pojawił się w pracy\n", atoi(argv[1]));
    fflush(stdout);
    signal(SIGINT, handler);

    while ( 1 )
    {
        start_service(sem);
        printf("Fryzjer %d obcina %s\n", atoi(argv[1]),hair); 
        fflush(stdout);
        sleep(strlen(hair));
        end_service(sem);
        // dettach_mem();  
    }
    
    return 0;
}
void tok(char* line){
    int i = 0;
    while (line[i]==' ')
    {
        i++;
    }
    
    int k=0;
    while (line[i]!=' ' && line[i]!='\n')
    {
        hair[k]=line[i];
        i++;
        k++;
    }
    hair[k]='\0';

    memmove(line,line+i+1,strlen(line)-i);
    fflush(NULL);
}
void start_service(int sem){
    struct sembuf sops[6];
    sops[0].sem_num = BARBERS;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;

    sops[1].sem_num = CHAIRS;
    sops[1].sem_op = -1;
    sops[1].sem_flg = 0;

    sops[2].sem_num = QUEUE;
    sops[2].sem_op = 1;
    sops[2].sem_flg = 0;

    sops[3].sem_num = CLIENTS;
    sops[3].sem_op = -1;
    sops[3].sem_flg = 0;

    sops[4].sem_num = QUEUE_SYNC;
    sops[4].sem_op = -1;
    sops[4].sem_flg = 0;

    sops[5].sem_num = QUEUE_LEN;
    sops[5].sem_op = -1;
    sops[5].sem_flg = 0;
    // print_all();
    if( semop(sem,sops,6) == -1){
        perror("semop in barber\n");
        fflush(stdout);
    };
//-----------------
    tok(shared_mem);
    struct sembuf sop;
    sop.sem_num = QUEUE_SYNC;
    sop.sem_flg = 0 ;
    sop.sem_op = 1;
    semop(sem, &sop,1);   
}
void end_service(int sem){
    struct sembuf sops[2];
    sops[0].sem_num = BARBERS;
    sops[0].sem_op = 1;
    sops[0].sem_flg = 0;
    sops[1].sem_num = CHAIRS;
    sops[1].sem_op = 1;
    sops[1].sem_flg = 0;
    semop(sem,sops,2);
}
void spawn_barber(int sem){
    struct sembuf sops;
    sops.sem_num = BARBERS;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    semop(sem,&sops,1);
}

void attach_mem(){
    key_t key = ftok("/",SHARED_ID);
    int shared_id = shmget(key, 2048, 0);
    if(shared_id == -1){
        perror("shmget\n");
        fflush(stdout);
        exit(-1);
    }
    shared_mem = shmat(shared_id, 0, 0);
    if (shared_mem == NULL){
        perror("shmat\n");
        fflush(stdout);
        exit(-1);
    }
}
void dettach_mem(){
    if(shmdt(shared_mem) == -1){
        perror("Detach error!\n");
        // exit(-1);
    };
}

void handler(int n){
    dettach_mem();
    exit(0);
}
