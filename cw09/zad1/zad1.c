#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#define REINDEER_NUMBER 9
#define ELVES_NUMBER 10
// 
pthread_mutex_t elves_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeers_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_sleep_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elves_cond_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elf_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t reindeer_cond_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;
int elves_waiting = 0;
int reindeers_waiting = 0;

void * reindeer_routine(void *){
    while (1)
    {
        sleep(rand()%6+5);
        // elf problem
        pthread_mutex_lock(&reindeers_mutex);
       
        if(reindeers_waiting < 8){
            reindeers_waiting ++;
            pthread_mutex_unlock(&reindeers_mutex);
            printf("Komunikat: Renifer: czeka %d reniferów na Mikołaja, %ld\n",reindeers_waiting, syscall(__NR_gettid));
            fflush(NULL);
            pthread_mutex_lock(&reindeer_cond_mutex);
            pthread_cond_wait(&reindeer_cond,&reindeer_cond_mutex);
            pthread_mutex_unlock(&reindeer_cond_mutex);
        }
        else if (reindeers_waiting == 8)
        {
            reindeers_waiting++;
            pthread_mutex_unlock(&reindeers_mutex);
            printf("Komunikat: Renifer: wybudzam Mikołaja %ld\n",syscall(__NR_gettid));
            fflush(NULL);
            pthread_cond_broadcast(&santa_sleep_cond);
            
            pthread_mutex_lock(&reindeer_cond_mutex);
            pthread_cond_wait(&reindeer_cond,&reindeer_cond_mutex);
            pthread_mutex_unlock(&reindeer_cond_mutex);
        }
    }
    
    return NULL;
}

void * elf_routine(void *){
    
    while (1)
    {
        sleep(rand()%4+2);
        // elf problem
        pthread_mutex_lock(&elves_mutex);

        if(elves_waiting < 2){
            elves_waiting ++;
            pthread_mutex_unlock(&elves_mutex);
            printf("Komunikat: Elf: czeka %d elfów na Mikołaja, %ld\n",elves_waiting, syscall(__NR_gettid));
            fflush(NULL);
        }
        else if (elves_waiting == 2)
        {
            elves_waiting++;
            pthread_mutex_unlock(&elves_mutex);
            printf("Komunikat: Elf: wybudzam Mikołaja %ld\n",syscall(__NR_gettid));
            fflush(NULL);
            pthread_cond_broadcast(&santa_sleep_cond);

            // waiting for help
            pthread_mutex_lock(&elves_cond_mutex);
            pthread_cond_wait(&elf_cond,&elves_cond_mutex);
            pthread_mutex_unlock(&elves_cond_mutex);

        }
        else{
            printf("Komunikat: Elf: samodzielnie rozwiązuję swój problem, %ld\n",syscall(__NR_gettid));
            fflush(NULL);
            pthread_mutex_unlock(&elves_mutex);
        }
        pthread_mutex_unlock(&elves_mutex);
    }
    
    return NULL;
}

int main(){
    int gifts = 0;
    pthread_t elves[ELVES_NUMBER];
    pthread_t reindeers[REINDEER_NUMBER];
    for(int i=0; i<ELVES_NUMBER; i++){
        pthread_create(&elves[i],NULL,elf_routine,NULL);
    } 
    for(int i=0; i<REINDEER_NUMBER; i++){
        pthread_create(&reindeers[i],NULL,reindeer_routine,NULL);
    }

    while (gifts < 3)
    {
        pthread_mutex_lock(&santa_mutex);
        while (reindeers_waiting < 9 && elves_waiting < 3)
        {
            pthread_cond_wait(&santa_sleep_cond,&santa_mutex);
        }
        pthread_mutex_lock(&reindeers_mutex);
        printf("Komunikat: Mikołaj budze się %d %d\n",elves_waiting, reindeers_waiting);
        if(reindeers_waiting >= 9){
            printf("Komunikat: Mikołaj dostarczam zabawki\n");
            fflush(NULL);
            sleep(rand()%3+2);
            reindeers_waiting = 0;
            pthread_cond_broadcast(&reindeer_cond);
            gifts ++;
        }
        pthread_mutex_unlock(&reindeers_mutex);

        pthread_mutex_lock(&elves_mutex);
        if(elves_waiting >= 3){
            printf("Komunikat: Mikołaj pomaga elfom\n");
            fflush(NULL);
            sleep(rand()%2+1);
            elves_waiting = 0;

            pthread_cond_broadcast(&elf_cond);
        }
        pthread_mutex_unlock(&elves_mutex);
        printf("Zasypiam\n");
        fflush(NULL);
        
        pthread_mutex_unlock(&santa_mutex);
    }
    
    return 0;
}