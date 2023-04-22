#include<ctype.h>
#include<unistd.h>
#include "common.h"

int handle_init();
void handle_list();
void receive_msg();
void handle_stop();
void handle_one(int other_id, char* str);
void handle_all(char* str);
void check_and_run( char** words);
void execute(cmd to_exec, char** parameters);
char** split_line(char* line);
char* read_line();
int is_number(char* str);
cmd convert(char* str);


key_t c_key;
key_t s_key;
int c_id;
int s_id;
int client_id;
pid_t child;

int main(){

    
    c_key = ftok(getenv("HOME"), getpid()); // zmienić
    s_key = ftok(getenv("HOME"), SERVER_ID); //error handling 
    c_id = msgget(c_key, IPC_CREAT| 0666);
    s_id = msgget(s_key, 0);
    // printf("%d \n", s_id);
    signal(SIGINT, handle_stop);
    client_id = handle_init();
    char* line = NULL;
    char** words = NULL;
    child = fork();

    while (child != 0)
    {
        printf(">>> ");
    // //     // receive_msg();
        line = read_line();
        words = split_line(line);
        check_and_run(words);
    }

    while (child == 0)
    {
        receive_msg();
    }
    

    // free(line);/// tu nigdy nie dojdzie
    // free(words);
    return 0;
}
int handle_init(){
    printf("INIT \n");
    msgbuf msg;
    msg.mtype = INIT;
    msg.key = c_key;
    printf("%d",msg.key);
    msgsnd(s_id, (void *) &msg, sizeof(msgbuf), 0);
    msgrcv(c_id, (void *) &msg, sizeof(msgbuf), 0, 0);

    if( msg.client_id == -1){
        perror("Client limit exceeded");
        handle_stop();
        exit(0);
    }
    printf("Client id %d \n", msg.client_id);
    return msg.client_id;
}
void handle_list(){
    // printf("LIST \n");
    msgbuf msg;
    msg.mtype = LIST;
    msg.key = c_key;
    msg.client_id = client_id;

    msgsnd(s_id, (void *) &msg, sizeof(msgbuf), 0);
    msgrcv(c_id, (void *) &msg, sizeof(msgbuf), 0, 0);

    printf("Clients %s \n", msg.content);
}

void handle_one(int other_id, char* str){
    // msgbuf msg;
    // msg.client_id = client_id;
    // msg.other_id = other_id;
    // strcpy(msg.content, str );
    // msgsnd(s_id, &msg, sizeof(msgbuf),0);
}
void handle_all(char* str){

    msgbuf msg;
    msg.client_id = client_id;
    msg.mtype = ALL;
    strcpy(msg.content, str );
    msgsnd(s_id, &msg, sizeof(msgbuf),0);
}
void handle_stop(){
    printf("STOP \n");
    msgbuf msg;
    msg.mtype = STOP;
    msg.key = c_key;
    if(getpid()!= child){
        kill(child, SIGKILL);
    }
    msgsnd(s_id, &msg, sizeof(msgbuf), 0);
    msgctl(c_id, IPC_RMID, NULL);
    exit(0);

}

void receive_msg(){
    msgbuf msg;
    printf(":C");
    msgrcv(c_id, &msg, sizeof(msgbuf),0,0);
    // while (msgrcv(c_id, &msg, sizeof(msgbuf), 0, IPC_NOWAIT) >= 0) {
    printf("\nmsg!!\n");
    if (msg.mtype == STOP) {
        printf("Received stop message, leaving..\n");
        handle_stop();
    } else {
        
        printf("Msg from: %d \n%s \n", msg.client_id, msg.content);
    }
        // }
    fflush(NULL);
}
void execute(cmd to_exec, char** parameters){
    switch (to_exec)
    {
    case LIST:
        handle_list();
        break;
    case ALL:
        handle_all(parameters[1]);
        break;
    case ONE:
        handle_one(atoi(parameters[1]), parameters[2]);
        break;
    case STOP:
        handle_stop();
        break;    
    default:
        perror("Invalid command\n");
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

    execute(command, words);
    
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

