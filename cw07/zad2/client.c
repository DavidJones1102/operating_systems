#include "common.h"
void spawn_client();
void attach_mem();
void dettach_mem();
char* gen_haircut();
void open_sem();

int client_nr;
char *shared_mem;
char hair[10];
sem_t** sem;

int main(int arg, char* argv[]){
    srand(time(NULL)*getpid());
    sem = malloc(SEM_SIZE*sizeof(sem_t* ));
    open_sem();

    strcpy(hair, gen_haircut());
    attach_mem(); //
    client_nr = atoi(argv[1]);

    spawn_client();
    dettach_mem(); //
    free(sem);
    return 0;
}

void spawn_client(){
    int len;

    if(sem_wait(sem[QUEUE_SYNC]) == -1){
        perror("[client] sem_wait\n");
        exit(0);
    }
    printf("Client %d chcę zrobić fryzure %s\n", client_nr, hair);
    fflush(stdout);
    sem_getvalue(sem[QUEUE],&len);
    if(len <= 0){
        printf("Client %d Nie ma dla mnie miejsca w kolejce :'( \n", client_nr);
        fflush(stdout);
        sem_post(sem[QUEUE_SYNC]);
        exit(0);      
    }
    if(sem_wait(sem[QUEUE]) == -1){
        perror("[client] sem_trywait\n");
        exit(0);
    }

    if(sem_post(sem[CLIENTS]) == -1){
        perror("[client] sem_post\n");
        exit(0);
    }

    strcat(shared_mem, hair);

    if(sem_post(sem[QUEUE_LEN]) == -1){
        perror("[client] sem_post\n");
        exit(0);
    }

    if(sem_post(sem[QUEUE_SYNC]) == -1){
        perror("[client] sem_post\n");
        exit(0);
    }
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
void open_sem(){
    sem[BARBERS] = sem_open(BARBERS_NAME, O_WRONLY);
    sem[CLIENTS] = sem_open(CLIENTS_NAME, O_WRONLY);
    sem[CHAIRS] = sem_open(CHAIRS_NAME, O_WRONLY);
    sem[QUEUE] = sem_open(QUEUE_NAME, O_WRONLY);
    sem[QUEUE_SYNC] = sem_open(QUEUE_SYNC_NAME, O_WRONLY);
    sem[QUEUE_LEN] = sem_open(QUEUE_LEN_NAME, O_WRONLY);
}
