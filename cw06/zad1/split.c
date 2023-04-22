#include<ctype.h>
#include<unistd.h>
#include "common.h"

void check_and_run( char** words);
char** split_line(char* line);
char* read_line();
int is_number(char* str);
cmd convert(char* str);



int main(){
    char* line = NULL;
    char** words = NULL;
    char* token;
        char* token1;
            char* token2;
    line = read_line();
    token = strtok(line, " ");
    token1 = strtok(NULL,"\n");

    printf("t %s \nl %s\n",token1, token);
    // words = split_line(line);
    // check_and_run(words);



    // free(line);/// tu nigdy nie dojdzie
    // free(words);
    return 0;
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

    cmd command = convert(words[0]);

    if( command == STOP && count!=1 ){
        perror("STOP don't take addintional arguments");
        return;
    }
    else if( command == LIST && count!=1 ){
        perror("LIST don't take addintional arguments");
        return;
    }
    else if( command == ALL && count!=2 ){
        perror("2ALL takes 1 argument");
        return;
    }
    else if(command == ONE && (count!=3 || is_number(words[1])!=1)){
        perror("2ONE takes 2 arguments int and string");
        return;
    }
    else if(command == INVALID){
        perror("Invalid command\n");
        return;
    }
    
}

cmd convert(char* str){
    if( !strcmp( str,"INIT") ){
        return INIT;
    }
    else if( !strcmp( str,"LIST") ){
        return LIST;
    }
    else if( !strcmp( str,"2ONE") ){
        return ONE;
    }
    else if( !strcmp( str,"2ALL") ){
        return ALL;
    }
    else if( !strcmp( str,"STOP") ){
        return STOP;
    }
    else{
        return INVALID;
    }
}

int is_number(char* str){
    int length = strlen(str);
    for (int i=0;i<length; i++)
        if (!isdigit(str[i])) 
            return 0;
        
    return 1;
}

char* read_line(){
    char* buff = NULL;
    size_t size = 0;
    getline(&buff, &size, stdin);
    if(buff == NULL){
        fprintf(stderr, "Unable to read line\n");
        exit(1);
    }

    return buff;
}

char** split_line(char* line){
    char **tokens = malloc(MAX_LENGTH*sizeof(char*));
    char* token;
    size_t position = 0;

    if(tokens == NULL){
        fprintf(stderr, "Allocation error\n");
        exit(1);
    }

    token = strtok(line, " \t\r\n\a");

    while (token!=NULL)
    {
        tokens[position] = token;
        position++;
        
        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position]=NULL;
    return tokens;
}

