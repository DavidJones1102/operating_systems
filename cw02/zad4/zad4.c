#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <string.h>
#include <libgen.h>

long long total_size = 0;
int calc(const char*path, const struct stat* ptr, int flag){
    if(!S_ISDIR(ptr->st_mode)){
        char filename[1024];
        strcpy(filename,path);
        printf("%ld %s\n", ptr->st_size, basename(filename));
        total_size += ptr->st_size;
    }

    return 0;
}
int main(int argc, char* argv[]){
    char *path = argv[1];

    int err = ftw( path,calc,1);
    if(err == -1){
        fprintf(stderr, "FTW error");
        return 1;
    }

    printf("%lld razem\n", total_size);
    return 0;
}