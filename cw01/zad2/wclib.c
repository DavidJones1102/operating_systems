#include "wclib.h"

size_t get_file_size(FILE* ptr);
int trash;

wc_array create(size_t size){
    wc_array array;
    array.size = size;
    array.current_index = 0;
    array.arr = calloc( size, sizeof(char*) );

    return array;
}
void count(wc_array* array, char* filename){

    char tmp[] = "/tmp/file_XXXXXX";
    int tmp_file = 0;
    tmp_file = mkstemp(tmp);
   
    if(tmp_file == 0){
       fprintf(stderr, "Failed to create temporary file\n");
    }

    char command[1024] = "";
    snprintf(command, 1024,"wc '%s' 1>'%s' 2>/dev/null", filename, tmp);
    trash = system(command);

    FILE* ptr = fopen(tmp,"r");
    size_t size = get_file_size(ptr);
    char* output = calloc(size, sizeof(char));
    trash = fread(output, sizeof(char), size, ptr);
    fclose(ptr);

    if(array->current_index<array->size){
        array->arr[array->current_index] = output;
        array->current_index++;
    }
    else{
        fprintf(stderr, "No more space in wc_array\n");
    }

    snprintf(command, 1024,"rm -f '%s'", tmp);
    trash = system(command);

}

char* get_index(wc_array* array, size_t index){
    if( index < array->current_index ){
        return array->arr[index];
    }

    return "";
}

void remove_index(wc_array* array, size_t index){
    if(index < array->current_index){
        free(array->arr[index]);
        memmove(array->arr+index,array->arr+index+1, (array->current_index-index-1)*sizeof(char*));
        array->current_index--;
    }
    else{
        fprintf(stderr, "Failed to remove record, index doesn't exist\n");
    }

}

void free_wc_array(wc_array* array){
    for(size_t i = 0; i < array->current_index; i++){
        free(array->arr[i]);
    }
    array->current_index = 0;
}


//helping functions

size_t get_file_size(FILE* ptr){
    fseek(ptr, 0, SEEK_END); // seek to end of file
    size_t size = ftell(ptr); // get current file pointer
    fseek(ptr, 0, SEEK_SET);

    return size;
}
