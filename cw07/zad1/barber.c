#include "common.h"
void open_sem();
void start_service(int sem);
void end_service(int sem);
void spawn_barber(int sem);
void attach_mem();
void dettach_mem();
void tok(char* line);
void handler(int n);
void get_haircut_from_shared_mem(int sem);


char *shared_mem;
char hair[10];

int main(int arg, char* argv[]){
    int sem = semget(ftok("/",SEM_ID),SEM_SIZE,0);
    spawn_barber(sem);
    printf("Fryzjer %d pojawił się w pracy\n", atoi(argv[1]));
    fflush(stdout);
    signal(SIGINT, handler);
    // char tmp[11] = "Aasdsf";

    attach_mem();
    while ( 1 )
    {
        // printf("b %d c %d ch %d q %d\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL));
        start_service(sem);
        get_haircut_from_shared_mem(sem);
        printf("Fryzjer %d obcina %s\n", atoi(argv[1]),hair); 
        fflush(stdout);
        sleep(strlen(hair));
        end_service(sem);
        // dettach_mem();  
    }
    
    return 0;
}
void tok(char* line){
    // printf("shm1 %s\n",line);
    int i = 0;
    // while (line[i]==' ')
    // {
    //     i++;
    // }
    
    int k=0;
    while (line[i]!=' ' && line[i]!='\n')
    {
        hair[k]=line[i];
        i++;
        k++;
    }
    // printf("i %d \n",i);
    hair[k]='\0';
    // printf("hair-- %c",line[i+1]);
    // printf("i - %d lne %ld \n",i,strlen(hair));

    memmove(line,line+i+1,strlen(line)-i);
        // printf("shm2 %s\n",line);
        // fflush(NULL);
    // return hair;
}
void start_service(int sem){
    struct sembuf sops[5];
    sops[0].sem_num = BARBERS;
    sops[0].sem_op = -1;
    sops[0].sem_flg = 0;
    sops[3].sem_num = CLIENTS;
    sops[3].sem_op = -1;
    sops[3].sem_flg = 0;
    sops[1].sem_num = CHAIRS;
    sops[1].sem_op = -1;
    sops[1].sem_flg = 0;
    sops[2].sem_num = QUEUE;
    sops[2].sem_op = 1;
    sops[2].sem_flg = 0;
    semop(sem,sops,4);
}
void end_service(int sem){
    struct sembuf sops[3];
    sops[0].sem_num = BARBERS;
    sops[0].sem_op = 1;
    sops[1].sem_num = CHAIRS;
    sops[1].sem_op = 1;
    // sops[2].sem_num = CLIENTS;
    // sops[2].sem_op = 1;
    semop(sem,sops,2);
    // printf("b %d c %d ch %d q %d\n\n",semctl(sem,0,GETVAL),semctl(sem,1,GETVAL),semctl(sem,2,GETVAL),semctl(sem,3,GETVAL));
}
void spawn_barber(int sem){
    struct sembuf sops;
    sops.sem_num = BARBERS;
    sops.sem_op = 1;
    semop(sem,&sops,1);
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
void get_haircut_from_shared_mem(int sem){
    struct sembuf sops;
    sops.sem_num = QUEUE_SYNC;
    sops.sem_flg = 0 ;
    sops.sem_op = -1;
    semop(sem, &sops,1);
    tok(shared_mem);
    sops.sem_num = QUEUE_SYNC;
    sops.sem_flg = 0 ;
    sops.sem_op = 1;
    semop(sem, &sops,1);   
}
void handler(int n){
    printf("--");
    fflush(stdout);
    dettach_mem();
    exit(0);
}
