#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include "zad1.h"
#include <time.h>


int main(int argc, char* argv[]){
    struct timespec start_lib, end_lib;
    struct timespec start_sys, end_sys;


    char before = argv[1][0];
    char after = argv[2][0];
    char* filename_from = argv[3];
    char* filename_to = argv[4];

    clock_gettime(CLOCK_REALTIME, &start_lib);
    replace_lib(before, after, filename_from, filename_to);
    clock_gettime(CLOCK_REALTIME, &end_lib);

    clock_gettime(CLOCK_REALTIME, &start_sys);
    replace_sys(before, after, filename_from, filename_to);
    clock_gettime(CLOCK_REALTIME, &end_sys);

    printf("Library functions runtime: %ld ns \n",end_lib.tv_nsec - start_lib.tv_nsec);
    printf("System functions runtime: %ld ns \n",end_sys.tv_nsec - start_sys.tv_nsec);
    return 0;
}

void replace_lib(char before, char after, char* filename_from, char* filename_to){

    FILE* from = fopen(filename_from, "r");
    FILE* to = fopen(filename_to, "w");

    if(!from || !to){
        fprintf(stderr, "Cannot open files\n");
        exit(0);
    }
    char c;
    while ( (c=fgetc(from)) != EOF)
    {
        if(c==before){
            c=after;
        }
        fputc(c,to);
    }
    
    fclose(from);
    fclose(to);
}

void replace_sys(char before, char after, char* filename_from, char* filename_to){
    int from = open(filename_from, O_RDONLY);
    int to = open(filename_to, O_WRONLY);

    if(from == -1|| to == -1){
        fprintf(stderr, "Cannot open files\n");
        exit(0);
    }

    char c;
    while ( read(from,&c,1)==1)
    {
        if(c==before){
            c=after;
        }
        write(to,&c,1);
    }
    
    close(from);
    close(to);
}