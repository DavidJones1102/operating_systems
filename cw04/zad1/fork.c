#include<stdlib.h>
#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>

enum command{IGNORE, HANDLER, MASK, PENDING, INVALID};
enum command parse(char* line);
void action(enum command cmd);
void handler(int signo);
char* read_line();
int sig = 10;


int main(){

    pid_t parent = getpid();
    fflush(NULL);
    char* line;
    size_t len;
    enum command cmd;
    size_t child;    

    while (1)
    {
        line = read_line();
        if(!strcmp(line,"exit\n")){
            break;
        }
        
        printf("Parent PID: %d\n",parent);
        fflush(NULL);
        cmd = parse(line);
        action(cmd);

        child = fork();

        if(cmd != INVALID && (cmd != PENDING || child!= 0)){
            printf("Raising signal %d\n",getpid());
            raise(sig);
            fflush(NULL);
        }
        
        if( cmd == PENDING )
        {
            sigset_t mask;
            sigpending(&mask);
            if (sigismember(&mask, sig))
                printf("Signal pending %d\n", getpid());
            else
                printf("Signal not pending %d\n", getpid());
            
        }


        if(parent != getpid()){
            break;
        }
    
    }
    


    free(line);
    return 0;
}

void handler(int signo){
    printf("Signal :) PID %d\n",getpid());
    fflush(NULL);
}

void action(enum command cmd){
    sigset_t mask;
    switch (cmd)
    {
    case IGNORE:
        printf("\nIgnore activated\n");
        signal(10,SIG_IGN);
        break;
    case HANDLER:
        sigemptyset(&mask);
        sigprocmask(SIG_SETMASK,&mask,NULL);
        printf("\nHandler activated\n");
        signal(10,handler);
        break;
    case MASK:
        printf("\nMask activated\n");
        sigfillset(&mask);
        sigprocmask(SIG_SETMASK,&mask,NULL);
        break;
    case PENDING:
        printf("\nPending activated\n");
        sigfillset(&mask);
        sigprocmask(SIG_SETMASK,&mask,NULL);
        break;    
    default:
        printf("\nInvalid arguments\n");
        break;
    }
}

enum command parse(char* line){
    if(!strcmp(line, "ignore\n")){
        return IGNORE;
    }
    else if(!strcmp(line, "handler\n")){
        return HANDLER;
    }
    else if(!strcmp(line, "mask\n")){
        return MASK;
    }
    else if(!strcmp(line, "pending\n")){
        return PENDING;
    }
    else{
        return INVALID;
    }

}

char* read_line(){
    char* buff = NULL;
    size_t size = 0;
    getline(&buff, &size, stdin);
    if(buff == NULL){
        fprintf(stderr, "Unable to read line\n");
        exit(1);
    }

    return buff;
}