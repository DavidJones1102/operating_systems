#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<time.h>
#include<fcntl.h>

double f(double x){
    return 4/(x*x+1);
}

double calculate_integral(double start, double stop, double dx){
    double result = 0;
    for(double x = start; x<stop; x+=dx){
        result += f(x)*dx;
        // printf("%lf \n",x);
        // fflush(NULL);
    }
    return result;
}

void write_to_fifo(double result, char* fifo){
    char buff[255];
    sprintf(buff,"%lf",result);
    int fd = open(fifo, O_WRONLY);
    write(fd, buff, 255);
}

int main(int argc, char* argv[]){
    if (argc != 5)
    {
        perror("Wrong number of arguments");
    }
    double start = strtod(argv[2], '\0');
    double stop = strtod(argv[3], '\0');
    double dx = strtod(argv[4], '\0');

    double result = calculate_integral(start, stop, dx);

    // printf("\n%lf\n",result);
    // fflush(NULL);
    
    char* fifo = argv[1];
    write_to_fifo(result, fifo);
    return 0;
}