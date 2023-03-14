#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef struct wc_array
{
    char **arr;
    size_t size;
    size_t current_index;
} wc_array;

wc_array (*create)(size_t);
void (*count)(wc_array*, char* );
char* (*get_index)(wc_array*, size_t );
void (*remove_index)(wc_array*, size_t);
void (*free_wc_array)(wc_array*);
