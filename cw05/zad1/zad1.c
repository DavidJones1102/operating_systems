#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
// To nie jest zadanie z zestawy tylko test funkcji popen
int main(int argc, char*argv[]){
    // FILE* fd = popen("fortune | cowsay","r");
    // char buff[2048];
    // size_t size = fread(buff,1,2048,fd);
    // buff[size] = 0;
    // printf("%s\n", buff);
     
    int fd[2];
    pipe(fd);
    pid_t ch = fork();
    char buff[1024];
    if (ch == 0){
        close(fd[0]);
        // buff[0] = 'a';
        // buff[1] ='l';
        // buff[2] = '\0';
        write(fd[1],"ala ma kota", 1024);

    }
    else{
        close(fd[1]);
        read(fd[0],&buff,1024);
        printf("%s \n",buff);
    }

    return 0;
}

