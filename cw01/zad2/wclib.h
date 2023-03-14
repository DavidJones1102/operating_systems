#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef struct wc_array
{
    char **arr;
    size_t size;
    size_t current_index;
} wc_array;

wc_array create(size_t size);

void count(wc_array* array, char* filename);

char* get_index(wc_array* array, size_t index);

void remove_index(wc_array* array, size_t index);

void free_wc_array(wc_array* array);