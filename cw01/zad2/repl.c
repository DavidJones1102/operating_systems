#include "repl.h"
#ifdef USE_DLL

void* load_dll(const char* filepath){
    void *handle = dlopen(filepath, RTLD_LAZY);

    if (!handle){
        fprintf(stderr, "DLL NOT FOUND \n");
        exit(1); 
    }

    count = (void(*)(wc_array* , char* ))dlsym(handle,"count");
    create = (wc_array(*)(size_t))dlsym(handle,"create");
    get_index = (char*(*)(wc_array* , size_t ))dlsym(handle,"get_index");
    free_wc_array = (void(*)(wc_array*))dlsym(handle,"free_wc_array");
    remove_index = (void(*)(wc_array* , size_t ))dlsym(handle,"remove_index");

    return handle;
}
#else
void* load_dll(char* filepath){return NULL;}
#endif

wc_array arr;
int status = 0;
int _;

int main(){
    void* handle = load_dll("./libwc.so");
    int running = 1;
    char* line = NULL;
    char** words = NULL;
    struct timespec time_before, time_after;
    struct tms tms_before, tms_after;

    while (running)
    {
        printf(">>> ");
        line = read_line();
        words = split_line(line);

        clock_gettime(CLOCK_REALTIME, &time_before);
        times(&tms_before);
        check_and_run(words);
        times(&tms_after);
        clock_gettime(CLOCK_REALTIME, &time_after);
        
        printf("EXECUTION TIME\n");
        printf("REAL %ld ns\n",time_after.tv_nsec-time_before.tv_nsec);
        printf("USER %ld tics\n",tms_after.tms_cutime-tms_before.tms_cutime);
        printf("SYSTEM %ld tics\n\n",tms_after.tms_cstime-tms_before.tms_cstime);

    }

    free(line);
    free(words);
    dlclose(handle);
    return 0;
}

void execute(enum command to_exec, char* parameters){
    switch (to_exec)
    {
    case INIT:
        arr = create(atol(parameters));
        status = 1;
        break;
    case COUNT:
        count(&arr, parameters);
        break;
    case SHOW:
        char msg[1024]="";
        strcpy(msg,get_index(&arr, atol(parameters)));
        printf("%s",msg);
        break;
    case DELETE:
        remove_index(&arr, atol(parameters));
        break;
    case DESTROY:
        free_wc_array(&arr);
        free(arr.arr);
        status = 0;
        break;    
    default:
        fprintf(stderr, "Invalid command\n");
        break;
    }

}

void check_and_run(char** words){
    int count = 0;
    while (words[count]!=NULL)
    {
        count++;
    }
    
    if(count <= 0){
        return;
    }

    enum command command_given = convert(words[0]);
    
    if(command_given>INIT && status == 0){
        fprintf(stderr, "You need to create wc_array first\n");
        return;
    }
    else if(command_given == INIT && status == 1){
        fprintf(stderr, "You have already inicialized structure\n");
        return;
    }
    else if( command_given == INVALID){
        fprintf(stderr, "Invalid command\n");
        return;
    }

    if( count==1 ){
        switch (command_given)
        {
        case INIT:
            fprintf(stderr, "You must specify size\n");
            return;
            break;
        case COUNT:
            fprintf(stderr, "You must specify path\n");
            return;
            break;
        case DELETE:
        case SHOW:
            fprintf(stderr, "You must specify index\n");
            return;
            break;
        case DESTROY:
            execute(DESTROY,"");
            break;  
        default:
            break;
        }
    }
    else if( count == 2 && command_given<DESTROY && (is_number(words[1]) == 1 || command_given == COUNT)){
        execute(command_given, words[1]);
    }
    else{
        fprintf(stderr, "Too many arguments\n");
        return;
    }
    
}

enum command convert(char* str){
    if( !strcmp( str,"INIT") ){
        return INIT;
    }
    else if( !strcmp( str,"DELETE") ){
        return DELETE;
    }
    else if( !strcmp( str,"SHOW") ){
        return SHOW;
    }
    else if( !strcmp( str,"DESTROY") ){
        return DESTROY;
    }
    else if( !strcmp( str,"COUNT") ){
        return COUNT;
    }
    else{
        return INVALID;
    }
}

int is_number(char* str){
    int length = strlen(str);
    for (int i=0;i<length; i++)
        if (!isdigit(str[i]))
        {
            return 0;
        }
    return 1;
}

char* read_line(){
    char* buff = NULL;
    size_t size = 0;
    _ = getline(&buff, &size, stdin);
    if(buff == NULL){
        fprintf(stderr, "Unable to read line\n");
        exit(1);
    }

    return buff;
}

char** split_line(char* line){
    char **tokens = malloc(BUFF_SIZE*sizeof(char*));
    char* token;
    size_t position = 0;

    if(tokens == NULL){
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }

    token = strtok(line, TOK_DELIM);

    while (token!=NULL)
    {
        tokens[position] = token;
        position++;
        
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position]=NULL;
    return tokens;
}

