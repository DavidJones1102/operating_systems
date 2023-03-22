#include<stdlib.h>
#include<stdio.h>
#include<dirent.h>
#include<unistd.h>
#include<sys/stat.h>
#include<string.h>
#include<sys/types.h>
#include<errno.h>
#include<limits.h>

char exepath[] = "/home/szymon/Pulpit/studia/sysopy/cw03/zad3/zad3.exe";
void error(int err, char* msg){
    if(err == -1){
        fprintf(stderr, "Error: %s",msg);
        exit(-1);
    }
}
int search(char path[PATH_MAX], const char filehead[255]){
    struct dirent* file; 
    struct stat stats;
 
    char buff[1024] = "";
    char filepath[PATH_MAX];

    DIR* dir = opendir(path);
    if( dir == NULL){
        perror("Cannot open directory\n");
        exit(-1);
    }
    int err;

    while ((file = readdir(dir)) != NULL)
    {
        err = stat(file->d_name, &stats);
        error(err, "Stat error");

        if(!strcmp(file->d_name,".") || !strcmp(file->d_name,"..") || !strcmp(file->d_name,".git")){
            continue;
        }
        if(S_ISDIR(stats.st_mode)){
            pid_t child = fork();
            if(child == 0){
                strcat(path,"/");
                strcat(path,file->d_name);
                fflush(NULL);
                err = execl(exepath,"zad3.exe",path, filehead, NULL);
                error(err,"Failed to exec function");
                break;
            }

        }
        else{
            FILE* f = fopen(file->d_name, "r"); //error handling
            fread(buff, sizeof(char), strlen(filehead), f);
    
            if(!strcmp(buff,filehead)){
                realpath(file->d_name,filepath);
                printf("%s PID: %d\n", filepath, getpid());
            }

            fclose(f);
            error(err,"cannot close file");
        }
    }
    
    closedir(dir);
    return 0;
    
}
int main(int argc, char*argv[]){
    if(argc!=3){
        fprintf(stderr,"Wrong number of arguments %d\n",argc);
        return -1;
    }
    else if (sizeof(argv[2])>255)
    {
        fprintf(stderr,"Wrong second argument");
        return -1;
    }

    char dirpath[PATH_MAX];
    strcpy(dirpath, argv[1]);
    char filehead[255];
    strcpy(filehead, argv[2]);

    chdir(dirpath);
    search(dirpath, filehead);

    return 0;
}