#include <ncurses.h>
#include <locale.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void resume(int sig_n){	//just to resume threads 
}
void init_grid(char *grid, pthread_t *threads);
void update_grid(pthread_t *threads);
void* routine(void* arg);

char *foreground;
char *background;

int main()
{
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode
	signal(SIGUSR1, resume);
	char *tmp;
	pthread_t *threads = malloc(sizeof(pthread_t)*grid_width*grid_height);
	foreground = create_grid();
	background = create_grid();
	init_grid(foreground,threads);

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		update_grid(threads);
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
void init_grid(char *grid, pthread_t *threads)
{
    for (int i = 0; i < grid_width * grid_height; ++i){
        cell *c = malloc(sizeof(c));
        c->col = i%grid_width;
        c->row = i/grid_width;
        grid[i] = rand() % 2 == 0;
        pthread_create(&threads[i],NULL,routine,(void *)c );
    }
}

void* routine(void* arg){
    int row = ((cell*) arg)->row;
    int col = ((cell*) arg)->col;
    free(arg);
    while (true)
    {
        pause();
		background[row*grid_width+col]=is_alive(row, col,foreground);
    }
    
    return NULL;
}

void update_grid(pthread_t *threads)
{
    for(int i=0; i<grid_width*grid_height;i++){
        pthread_kill(threads[i], SIGUSR1);
    }
}