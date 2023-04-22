#include<ctype.h>
#include<unistd.h>
#include "common.h"

int handle_init();
void handle_list();
void receive_msg();
void handle_stop();
void handle_one(int other_id, char* str);
void handle_all(char* str);
void execute( char* line);
char* read_line();
void fill_time(msgbuf* msg);
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

    signal(SIGINT, handle_stop);
    client_id = handle_init();
    char* line = NULL;
    char** words = NULL;
    child = fork();

    while (child != 0)
    {
        printf(">>> ");
        line = read_line();
        execute(line);
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
    fill_time(&msg);
    msg.mtype = INIT;
    msg.key = c_key;

    msgsnd(s_id, (void *) &msg, MSG_SIZE, 0);
    msgrcv(c_id, (void *) &msg, MSG_SIZE, 0, 0);

    if( msg.client_id == -1){
        perror("Client limit exceeded");
        handle_stop();
        exit(0);
    }
    printf("Client id %d \n", msg.client_id);
    return msg.client_id;
}
void handle_list(){
    msgbuf msg;
    fill_time(&msg);
    msg.mtype = LIST;
    msg.key = c_key;
    msg.client_id = client_id;

    msgsnd(s_id, (void *) &msg, MSG_SIZE, 0);

}

void handle_one(int other_id, char* str){
    msgbuf msg;
    fill_time(&msg);
    msg.client_id = client_id;
    msg.other_id = other_id;
    msg.mtype = ONE;
    strcpy(msg.content, str );
    msgsnd(s_id, &msg, MSG_SIZE,0);
}
void handle_all(char* str){

    msgbuf msg;
    fill_time(&msg);
    msg.client_id = client_id;
    msg.mtype = ALL;
    strcpy(msg.content, str );
    msgsnd(s_id, &msg, MSG_SIZE,0);
}
void handle_stop(){
    printf("STOP \n");
    msgbuf msg;
    fill_time(&msg);
    msg.mtype = STOP;
    msg.key = c_key;
    msg.client_id = client_id;
    if(getpid()!= child){
        kill(child, SIGKILL);
    }
    msgsnd(s_id, &msg, MSG_SIZE, 0);
    msgctl(c_id, IPC_RMID, NULL);
    exit(0);

}

void receive_msg(){
    msgbuf msg;
    msgrcv(c_id, &msg, MSG_SIZE,-5,0);

    if (msg.mtype == STOP) {
        printf("\nReceived stop message, leaving..\n");
        handle_stop();
    } else if(msg.mtype != LIST){
        printf("\nTime - %02d:%02d:%02d: ",msg.time.tm_hour,msg.time.tm_min,msg.time.tm_sec);
        printf("Msg from: %d \n%s \n", msg.client_id, msg.content);
        printf(">>>");
    }
    else{
        printf("Clients %s \n", msg.content);
    }
      
    fflush(NULL);
}
void fill_time(msgbuf* msg){
    time_t curr_time = time(NULL);
    msg->time=*localtime(&curr_time);
}
void execute(char* line){
    if(strcmp(line,"\n") == 0){
        return;
    }
    char* first = strtok(line, " \n");
    char* second;
    char* third;
    cmd command = convert(first);

    switch (command)
    {
    case LIST:
        handle_list();
        break;
    case ALL:
        second = strtok(NULL,"\n");
        handle_all(second);
        break;
    case ONE:
        second = strtok(NULL," ");
        if( second == NULL){
            perror("Too few arguments\n");
            return;
        }
        if( is_number(second)!=1){
            perror("Wrong client id\n");
            return;
        }
        third = strtok(NULL, "\n");
        if( third == NULL){
            perror("Too few arguments\n");
            return;
        }
        handle_one(atoi(second), third);
        break;
    case STOP:
        handle_stop();
        break;    
    default:
        perror("Invalid command\n");
        break;
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


