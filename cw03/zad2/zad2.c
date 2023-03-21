#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

int main(int argc, char*argv[]){
    if(argc!=2){
        fprintf(stderr,"Wrong number of arguments");
        return -1;
    }
    char* dirpath = argv[1];
    printf("%s",dirpath);
    fflush(stdout);
    execl("/bin/ls","/bin/ls",dirpath,NULL);
    return 0;
}