#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
void reverse_file(char* filename_from, char* filename_to, size_t buff_size);
size_t get_file_size(FILE* f);
void reverse(char* str);


int main(int argc, char* argv[]){
    char* filename_from = argv[1];
    char* filename_to = argv[2];

    struct timespec start_1, end_1;
    struct timespec start_1024, end_1024;
    
    clock_gettime(CLOCK_REALTIME,&start_1);
    reverse_file(filename_from, filename_to, 1);
    clock_gettime(CLOCK_REALTIME,&end_1);

    clock_gettime(CLOCK_REALTIME,&start_1024);
    reverse_file(filename_from, filename_to, 1024);
    clock_gettime(CLOCK_REALTIME,&end_1024);

    printf("Buff size 1: %ld ns \n",end_1.tv_nsec - start_1.tv_nsec);
    printf("Buff size 1024: %ld ns \n",end_1024.tv_nsec - start_1024.tv_nsec);
   
    return 0;
}

void reverse_file(char* filename_from, char* filename_to, size_t buff_size){

    char buff[buff_size+1];
    FILE* from = fopen(filename_from, "r");
    FILE* to = fopen(filename_to, "w");
    
    if(!from || !to){
        fprintf(stderr,"Cannot open files");
        exit(0);
    }

    size_t size = get_file_size(from);
    size_t count = size/buff_size;
    size_t r = size%buff_size;
    char c;
    int end;
    for (int i=1;i<=count;i++)
    {
        fseek(from, -buff_size*i, SEEK_END);
        end = fread(buff, sizeof(char), buff_size, from);
        reverse(buff);
        fwrite(buff, sizeof(char), buff_size, to);

    }

    fseek(from, -buff_size*count-r, SEEK_END);
    end = fread(buff, sizeof(char), r, from);
    buff[end] = 0;
    reverse(buff);
    fwrite(buff, sizeof(char), r, to);


    fclose(from);
    fclose(to);
}

size_t get_file_size(FILE* f){
    fseek(f,0,SEEK_END);
    size_t size = ftell(f);
    fseek(f,0,SEEK_SET);

    return size;
}

void reverse(char* str){
    int n = strlen(str);
    int tmp;
    for(int i=0; i<n/2;i++){
        tmp = str[i];
        str[i] = str[n-i-1];
        str[n-i-1] = tmp;
    }
}