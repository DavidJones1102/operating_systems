#include<stdlib.h>
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>

void handler_1(int sig_no, siginfo_t *info, void *secret) {
    printf("Signal number: %d\n", info->si_signo);
    printf("Signal code %d\n", info->si_code);
    printf("Sending process ID %d\n", info->si_pid);
    printf("Real user ID of sending process %d\n", info->si_uid);
    printf("User time consumed %d\n", (int)info->si_utime);
    printf("System time consumed %d\n", (int)info->si_stime);
}
void handler_2(int signo){
    printf("Signal :), PID: %d \n",getpid());
    fflush(NULL);
}
void siginfo_test(){
    struct sigaction act;
    act.sa_sigaction = &handler_1;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    printf("SIGUSR1 sent with SA_SIGINFO\n");
    kill(getpid(), SIGUSR1);
}

void nocldstop_test(){
    struct sigaction act;
    act.sa_handler = &handler_2;
    act.sa_flags = 0;
    sigfillset(&act.sa_mask);
    sigaction(SIGCHLD, &act, NULL);

    pid_t child = fork();
    
    if(child == 0){
        while (1)
        {
            sleep(1);
        }
        
    }
    else{
        sleep(1);
        printf("\nChild stopped without SA_NOCLDSTOP\n");
        kill(child,SIGSTOP);
        sleep(1);
        printf("Child killed without SA_NOCLDSTOP\n");
        kill(child,SIGKILL);
        sleep(1);
    }

    act.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &act, NULL);
    child = fork();
    if(child == 0){
        while (1)
        {
            sleep(1);
        }
        
    }
    else{
        sleep(1);
        printf("\nChild stopped with SA_NOCLDSTOP\n");
        kill(child,SIGSTOP);
        sleep(1);
        printf("Child killed with SA_NOCLDSTOP\n");
        kill(child,SIGKILL);
        sleep(1);
    }
    
}

void resethand_test(){
    pid_t parent = getpid();
    struct sigaction act;
    act.sa_handler = &handler_2;
    act.sa_flags = 0;
    sigfillset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);


    printf("\nSIGUSR1 sent first time no resethand flag\n");
    fflush(NULL);
    kill(parent, SIGUSR1);
    sleep(1);
    printf("SIGUSR1 sent second time no resethand flag\n");
    fflush(NULL);
    kill(parent, SIGUSR1);
    sleep(1);


    act.sa_flags = SA_RESETHAND;
    sigaction(SIGUSR1, &act, NULL);
    
    printf("SIGUSR1 sent first time resethand flag\n");
    fflush(NULL);
    kill(parent, SIGUSR1);
    sleep(1);
    printf("SIGUSR1 sent second time resethand flag\n");
    fflush(NULL);
    kill(parent, SIGUSR1);
    sleep(1);
}

int main(){
    siginfo_test();
    nocldstop_test();
    resethand_test();
    return 0;
}