#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
// To nie jest zadanie z zestawy tylko test funkcji popen
int main(int argc, char*argv[]){
    FILE* fd = popen("fortune","r");
    char buff[2048];
    size_t size = fread(buff,1,2048,fd);
    buff[size] = 0;
    printf("%s\n", buff);

    return 0;
}

