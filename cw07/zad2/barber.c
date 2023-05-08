#include "common.h"

void start_service();
void end_service();
void spawn_barber();
void attach_mem();
void dettach_mem();
void tok(char* line);
void handler(int n);
void open_sem();

char *shared_mem;
char hair[10];
sem_t** sem;

int main(int arg, char* argv[]){
    sem = malloc(SEM_SIZE*sizeof(sem_t* ));
    open_sem();

    attach_mem();
    spawn_barber();
    printf("Fryzjer %d pojawił się w pracy\n", atoi(argv[1]));
    fflush(stdout);
    signal(SIGINT, handler);

    while ( 1 )
    {
        start_service();
        printf("Fryzjer %d obcina %s\n", atoi(argv[1]),hair); 
        fflush(stdout);
        sleep(strlen(hair));
        end_service();
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
void start_service(){
    sem_wait(sem[CHAIRS]);
    sem_wait(sem[CLIENTS]);
    sem_wait(sem[BARBERS]);
    sem_wait(sem[QUEUE_SYNC]);
    sem_post(sem[QUEUE]);
    sem_wait(sem[QUEUE_LEN]);

//-----------------
    tok(shared_mem);
    sem_post(sem[QUEUE_SYNC]);
}
void end_service(){
    sem_post(sem[BARBERS]);
    sem_post(sem[CHAIRS]);
}
void spawn_barber(){
    sem_post(sem[BARBERS]);
}

void attach_mem(){
    int shared_fd = shm_open(SHARED_NAME, O_RDWR, 0666);
    if(shared_fd == -1){
        perror("[client] shm_open error \n");
        exit(0);
        // raise(SIGKILL);//!
    }

    if(ftruncate(shared_fd, 2048) == -1){
        perror("frtuncate error \n");
        fprintf(stderr, "Value of errno: %s\n", strerror(errno));
        // raise(SIGKILL);
        exit(0);
    }
    shared_mem = mmap(NULL, 2048, PROT_READ|PROT_WRITE, MAP_SHARED, shared_fd, 0);
}
void dettach_mem(){
    if(munmap(shared_mem,2048) == -1){
        perror("[main] munmap\n");
    }
}

void handler(int n){
    dettach_mem();
    free(sem);
    exit(0);
}
void open_sem(){
    sem[BARBERS] = sem_open(BARBERS_NAME, O_WRONLY);
    sem[CLIENTS] = sem_open(CLIENTS_NAME, O_WRONLY);
    sem[CHAIRS] = sem_open(CHAIRS_NAME, O_WRONLY);
    sem[QUEUE] = sem_open(QUEUE_NAME, O_WRONLY);
    sem[QUEUE_SYNC] = sem_open(QUEUE_SYNC_NAME, O_WRONLY);
    sem[QUEUE_LEN] = sem_open(QUEUE_LEN_NAME, O_WRONLY);
}
