#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
#include<signal.h>
#include<time.h>

void handler_1(int sig_no, siginfo_t *info, void *secret);
void set_handler();
void action(int mode);
void print_time();
int counter = 0;
int f_print_time;
int main(){
    printf("%d \n",getpid());
    set_handler();
    while(1){
        if(f_print_time==1){
            print_time();
        }
        sleep(1);
    }
}

void handler_1(int sig_no, siginfo_t *info, void *secret) {
    printf("Signal catched :)\n");
    counter++;
    kill(info->si_pid,SIGUSR1);
    action(info->si_value.sival_int);

}

void set_handler(){
    struct sigaction act;
    act.sa_sigaction = &handler_1;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);
}

void action(int mode){
    switch (mode)
    {
    case 1:
        f_print_time = 0;
        for(int i=1; i<101; i++) printf("%d ",i);
        printf("\n");
        break;
    case 2:
        f_print_time = 0;
        print_time();
        break;
    case 3:
        f_print_time = 0;
        printf("%d \n",counter);
        break;
    case 4:
        f_print_time = 1;  
        break;
    case 5:
        exit(0);
        break;
    default:
        fprintf(stderr, "Uknown command\n");
        break;
    }
    
}

void print_time(){
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    printf ( "Current local time and date: %s\n", asctime (timeinfo) );
    
}