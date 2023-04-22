#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<time.h>
#include<fcntl.h>

#define FIFO "./fifo"
#define EXE_PATH "/home/szymon/Pulpit/studia/sysopy/cw05/zad3/integral.exe"
double f(double x){
    return 4/(x*x+1);
}

double calculate_integral(double start, double stop, double dx){
    double result = 0;
    for(double x = start; x<stop; x+=dx){
        result += f(x)*dx;
    }
    return result;
}

struct timespec timespec_diff(struct timespec start, struct timespec end) {
    struct timespec out;

    if ((end.tv_nsec-start.tv_nsec)<0) {
            out.tv_sec = end.tv_sec-start.tv_sec-1;
            out.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    }
    else {
            out.tv_sec = end.tv_sec-start.tv_sec;
            out.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return out;
}

int main(int argc, char* argv[]){
    if(argc!=3) {
        perror("Wrong number of arguments in main\n");
        exit(-1);
    }
    double dx = strtod(argv[1],'\0');
    int n = atoi(argv[2]); // liczba procesów potomnych
    double step = 1.0 / n;
    char start[100];
    char stop[100];
    double result = 0;
    char buff[2048];

    mkfifo(FIFO,0666);

    struct timespec time_start, time_end;
    clock_gettime(CLOCK_REALTIME, &time_start);
    // printf("nnnnnnnnnnn%d",n);
    pid_t child;
    for(int i=0; i<n; i++){
        child = fork();
        if( child == 0){
            sprintf(start,"%lf",i*step);
            sprintf(stop,"%lf",i*step+step);
            // printf("%s %s\n",start,stop);
            execl(EXE_PATH,"integral.exe","fifo",start,stop,argv[1],NULL);
            perror("Execl error");
            // exit(0);
        }        
    }


    int fifo = open(FIFO, O_RDONLY);
    int i = 0;
    while (i < n) {
        size_t size = read(fifo, buff, 2048);
        buff[size] = 0;

        char delim[] = "\n";
        char* token;

        token = strtok(buff, delim);
        while( token != NULL) {
            result += strtod(token, NULL);
            i++;
            token = strtok(NULL, delim);
        }
    }

    clock_gettime(CLOCK_REALTIME, &time_end);
    struct timespec out = timespec_diff(time_start,time_end); 
    printf("result %lf\n",result);
    printf("Time taken: %lds %ldns\n", out.tv_sec, out.tv_nsec);
    close(fifo);
    system("rm -f fifo");

    
    return 0;
}
