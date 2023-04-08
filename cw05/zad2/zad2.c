#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<time.h>

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
        perror("Wrong number of arguments\n");
        exit(-1);
    }
    double dx = strtod(argv[1],'\0');
    int n = atoi(argv[2]); // liczba procesów potomnych
    double step = 1.0 / n;
    double result = 0;
    char buff[255];
    int fd[2];
    int* out_pipes = malloc(sizeof(int)*n);

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);

    for(int i=0; i<n; i++){
        pipe(fd);
        pid_t child = fork();
        if( child != 0){
            close(fd[1]);
            out_pipes[i] = fd[0];
        }
        else{
            close(fd[0]);
            result = calculate_integral(i*step,(i+1)*step,dx);
            sprintf(buff,"%lf",result);
            write(fd[1],buff,255);
            close(fd[1]);
            exit(0);
        }
        
    }

    for(int i=0; i<n; i++){
        read(out_pipes[i],buff,255);
        result += strtod(buff,'\0');
        close(out_pipes[i]);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec out = timespec_diff(start,end); 
    printf("%lf\n",result);
    printf("Time taken: %lds %ldns\n", out.tv_sec, out.tv_nsec);
    
    return 0;
}
