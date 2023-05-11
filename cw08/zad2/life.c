#include <ncurses.h>
#include <locale.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void resume(int sig_n){	//just to resume threads 
}
void init_grid(char *grid, pthread_t *threads, int n);
void update_grid(pthread_t *threads, int n);
void* routine(void* arg);

char *foreground;
char *background;

int main(int argc, char*argv[])
{
	if(argc != 2){
		perror("Wrong number of arguments\n");
		exit(0);
	}
	int n = atoi(argv[1]);
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode
	signal(SIGUSR1, resume);
	char *tmp;
	pthread_t *threads = malloc(sizeof(pthread_t)*n);
	foreground = create_grid();
	background = create_grid();
	init_grid(foreground,threads, n);
	usleep(500 * 1000);
	while (true)
	{
		draw_grid(foreground);
		// Step simulation
		update_grid(threads, n);
		usleep(500 * 1000);

		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);
	free(threads);

	return 0;
}
void init_grid(char *grid, pthread_t *threads, int n)
{

    for (int i = 0; i < grid_width * grid_height; ++i){
        grid[i] = rand() % 2 == 0;
    }

	int cell_for_thread = (grid_height*grid_width)/n;
	int additional_threads = (grid_height*grid_width)%n;
	int added = 0;			
	int next = 0;
	for(int i = 0; i<n;i++){
        cell *c = malloc(sizeof(cell));
		c->start = i*cell_for_thread+added;
		if(additional_threads > added){
        	c->end = (i+1)*cell_for_thread+1+added;
			added++;
		}
		else{
			c->end = (i+1)*cell_for_thread+added;
		}
		next+=cell_for_thread;

	// printf("s %d e %d\n",c->start, c->end);
        pthread_create(&threads[i],NULL,routine,(void *)c );
	}
}

void* routine(void* arg){
    int start = ((cell*) arg)->start;
    int end = ((cell*) arg)->end;
    free(arg);

    while (true)
    {

        pause();
		for(int i = start; i<end; i++){
			// printf();

		background[i]=is_alive(i/grid_width, i%grid_width,foreground);
			// printf("%d i %d\n",background[i],i);
		// fflush(NULL);
		}
    }
    
    return NULL;
}

void update_grid(pthread_t *threads, int n)
{
    for(int i=0; i<n;i++){
        pthread_kill(threads[i], SIGUSR1);
    }
}