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
void send(cmd type, char* content, int other_id);
char randomC();
mqd_t create_queue(const char* name);
void get_name();
char* to_string(cmd command);

int client_id;
pid_t child;
char name[LEN];
mqd_t c_queue;
mqd_t s_queue;


int main(){
    srand(time(NULL));
    get_name();
    s_queue = mq_open(SERVER, O_RDWR);
    c_queue = create_queue(name);
        printf("c %d \n",c_queue);
        printf("s %d \n",s_queue);
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
        // receive_msg();
    }
    

    // free(line);/// tu nigdy nie dojdzie
    // free(words);
    return 0;
}
void send(cmd type, char* content, int other_id){
    char msg[MAX_LENGTH];
    sprintf(msg, "%s %d %d %s \n",to_string(type), client_id, other_id, content);
    printf("msg %s \n",msg);
    mq_send(s_queue, msg, MSG_SIZE, 0);
}
int handle_init(){
    printf("INIT \n");
    char msg[6];

    send(INIT,name,-1);
    mq_receive(c_queue,msg, sizeof(char)*7, NULL);
    printf("INIT2 %s-\n",msg);
    fflush(NULL);
    int id = atoi(msg);
    if( id == -1){
        perror("Client limit exceeded");
        handle_stop();
        exit(0);
    }
    printf("Client id %d \n", id);
    return id;
}
void handle_list(){
    send(LIST, "",-1);
}

void handle_one(int other_id, char* str){
    send(ONE,str,other_id);
}
void handle_all(char* str){
    send(ALL,str,-1);
}
void handle_stop(){
    printf("STOP \n");
    if(getpid()!= child){
        kill(child, SIGKILL);
    }
    send(STOP,"",-1);
    mq_close(c_queue);
    mq_unlink(name);
    exit(0);
}

void receive_msg(){
    char msg[MAX_LENGTH];
    mq_receive(s_queue,msg,MSG_SIZE,NULL);
    char* first = strtok(msg, " \n");
    cmd command;
    char* second;
    char* content;
    if( first!= NULL){
        command = convert(first);
        second = strtok(NULL," \n");                 
    }
    if(second != NULL){
        content = strtok(NULL, "\n");
    }
    
    if (command == STOP) {
        printf("\nReceived stop message, leaving..\n");
        handle_stop();
    } else if(command != LIST){
        printf("Msg from: %s \n%s \n", second, content);
        printf(">>>");
    }
    else{
        printf("Clients %s \n", content);
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
char* to_string(cmd command){
    switch (command)
    {
    case INIT:
        return "INIT";
        break;
    case LIST: 
        return "LIST";
        break;
    case STOP:
        return "STOP";
        break;
    case ALL:
        return "ALL";
        break;
    case ONE:
        return "ONE";
        break;
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

char randomC() {
    return rand() % ('Z' - 'A' + 1) + 'A';
}

mqd_t create_queue(const char* name) {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = CLIENTS;
    attr.mq_msgsize = MSG_SIZE;
    attr.mq_curmsgs = 0;
    return mq_open(name, O_RDWR | O_CREAT, 0666, &attr);
}
void get_name() {
    char temp[LEN];
    temp[0] = '/';

    for (int i = 1; i < LEN; i++) {
        temp[i] = randomC();
    }
    strcpy(name,temp);
    printf("n %s\n",name);
}