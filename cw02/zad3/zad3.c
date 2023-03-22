#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

int main(){
    char dir_name[1024]="/home/szymon/Pulpit/studia/sysopy/cw02";
    // getcwd(dir_name, sizeof(dir_name));
    struct dirent* file;
    struct stat stats;
    long long total_size = 0;

    DIR* d = opendir(dir_name);

    if( d == NULL){
        fprintf(stderr, "Cannot open directory");
    }
    
    while ((file = readdir(d)) != NULL)
    {
        stat(file->d_name, &stats);
        if(!S_ISDIR(stats.st_mode)){
            printf("%ld %s\n", stats.st_size, file->d_name);
            total_size += stats.st_size;
        }

    }
    printf("%lld razem\n",total_size);
    closedir(d);

    return 0;
}
