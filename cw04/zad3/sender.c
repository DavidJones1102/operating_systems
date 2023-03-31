#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
void handler_1(int sig_no) {
    printf("Signal delivered :)\n");
    fflush(NULL);   
}
// void handler_1(int sig_no, siginfo_t *info, void *secret) {
//     printf("Signal delivered :)\n");
//     printf("Sending process ID %d\n", info->si_pid);

// }

// void set_handler(){
//     struct sigaction act;
//     act.sa_sigaction = &handler_1;
//     act.sa_flags = SA_SIGINFO;
//     sigaction(SIGUSR1, &act, NULL);
// }


int main(int argc, char* argv[]){
    printf("%d\n",getpid());
    if(argc <= 1){
        printf("Too few arguments");
        exit(-1);
    }
    pid_t catcher = atoi(argv[1]);
    // set_handler();
    union sigval value;
    sigset_t mask;
    signal(SIGUSR1,handler_1);
    sigfillset(&mask );
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT);
    for(int i=2; i<argc; i++){
        value.sival_int = atoi(argv[i]);
        sigqueue(catcher, SIGUSR1, value);
        sigsuspend(&mask);

    }
    
}