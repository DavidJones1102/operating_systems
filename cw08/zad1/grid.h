#pragma once
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#define grid_width 30
#define grid_height 30

typedef struct cell
{
    int col;
    int row;
}cell;

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
bool is_alive(int row, int col, char *grid);

