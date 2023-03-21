#include<stdlib.h>
#include<stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
void search_dir(char* dirpath){
    DIR* curr_dir = opendir(dirpath);
    if(curr_dir == NULL){
        fprintf(stderr, "Cannot open directory");//perror
        exit(-1);
    }
    
    struct dirent* curr_file;
    struct stat file_stats;

    while ((curr_file=readdir(curr_dir)) != NULL)
    {
        stat(curr_file->d_name, &file_stats);
        if(S_ISDIR(file_stats.st_mode)){
            // fork()
        }                
    }
    
}
int main(int argc, char*argv[]){
    if(argc!=3){
        fprintf(stderr,"Wrong number of arguments");
        return -1;
    }
    else if (sizeof(argv[2])>255)
    {
        fprintf(stderr,"Wrong second argument");
        return -1;
    }
    
    char* dirpath = argv[1];
    char* filehead = argv[2];
    printf("%s",dirpath);
    fflush(stdout);
    
    return 0;
}