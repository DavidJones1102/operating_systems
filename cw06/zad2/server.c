#include<unistd.h>
#include "common.h"
mqd_t s_queue;
char* clients[CLIENTS];
int next_client = 0;
// error handling

void handle_init(msgbuf msg);
void handle_list(msgbuf msg);
void handle_all(msgbuf msg);
void handle_one(msgbuf msg);
void handle_stop(msgbuf msg);
void SIGINT_handler(int n);
void save_log(msgbuf msg);
char randomC();
char* to_string(cmd command);
void send(msgbuf msg);
cmd convert(char* str);
mqd_t create_queue(const char* name);

int main(){

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = CLIENTS;
    attr.mq_msgsize = MSG_SIZE;
    attr.mq_curmsgs = 0;
    s_queue = mq_open(SERVER, O_RDONLY | O_CREAT, 0666, &attr);
    if(s_queue == -1){
        perror("mq_open\n" );
        exit(0);
    }

    signal(SIGINT, SIGINT_handler);

    for( int i = 0; i < CLIENTS; i++){
        clients[i] = NULL;
    }
    char msg[MAX_LENGTH];
    msgbuf struct_msg;

    while (1)
    {

        mq_receive(s_queue, msg, MSG_SIZE, NULL);
        fflush(NULL);
        struct_msg.mtype = convert(strtok(msg," "));
        struct_msg.client_id = atoi(strtok(NULL," "));
        struct_msg.other_id = atoi(strtok(NULL," "));
        strcpy(struct_msg.content, strtok(NULL,"\n"));
        printf("---%s\n",to_string(struct_msg.mtype));
        switch (struct_msg.mtype) {
            case INIT:
                handle_init(struct_msg);
                break;
            case LIST:
                handle_list(struct_msg);
                // store_message_logs(struct_msg);
                break;
            case ALL:
                handle_all(struct_msg);
                // store_message_logs(struct_msg);
                break;
            case ONE:
                handle_one(struct_msg);
                // store_message_logs(struct_msg);
                break;
            case STOP:
                handle_stop( struct_msg );
                // store_message_logs(struct_msg);
                break;
            default:
                printf("Unexpected message type!\n");
        }
    }
    
    return 0;
}


void handle_init(msgbuf msg){
    printf("INIT \n");
    while (clients[next_client] != NULL && next_client<CLIENTS)
    {
        next_client++;
    }
    
    if( next_client < CLIENTS){
        msg.client_id = next_client; //yyy??
        clients[next_client] = malloc(sizeof(char)*(strlen(msg.content)+1));
        clients[next_client] = msg.content;
        next_client++;
    }
    else{
        msg.client_id = -1;
    }

    fflush(NULL);
    mqd_t c_queue = mq_open(msg.content, O_RDWR);
    char id[6];
    sprintf(id,"%d",next_client-1);
    printf("msg c:%s \n",id);
    mq_send(c_queue, id, sizeof(char)*7, 0);
    mq_close(c_queue);
    save_log(msg);
    
}
void handle_list(msgbuf msg){
    printf("LIST\n");
    // int c_id = msgget(clients[msg.client_id], 0);
    // char list[MAX_LENGTH]="";
    // char temp[13];
    // for(int i=0; i<CLIENTS; i++){
    //     if(clients[i]!=-1 && i!=msg.client_id){
    //         sprintf(temp,"%d ",i);
    //         strcat(list, temp);
    //     }
    // }

    // strcpy(msg.content, list);
    // msgsnd(c_id,&msg, MSG_SIZE, 0);
    // save_log(msg);
    
}
void handle_all(msgbuf msg){
    printf("2ALL\n");
    // for(int i=0; i<CLIENTS; i++){
    //     if(clients[i]!=-1 && i!=msg.client_id){
    //         int other_id = msgget(clients[i],0);
    //         msgsnd(other_id, &msg, MSG_SIZE,0);
    //     }
    //     save_log(msg);
    // }
}

void handle_one(msgbuf msg){
    printf("2ONE\n");
    // if(clients[msg.other_id]!=-1 ){
    //     int other_id = msgget(clients[msg.other_id],0);
    //     msgsnd(other_id, &msg, MSG_SIZE,0);
    // }
    // if(clients[msg.other_id] != -1){
    //     int c_id = msgget(clients[msg.other_id],0);
    //     msgsnd(c_id, &msg, MSG_SIZE,0);
    // }
    save_log(msg);
}
void handle_stop( msgbuf msg){
    // mq_close(clients[msg.client_id]);
    clients[msg.client_id] = NULL;
    if( msg.client_id < next_client){
        next_client = msg.client_id;
    }
    save_log(msg);
}

void SIGINT_handler(int n){
    printf("handler :)\n");
    for(int i=0; i<CLIENTS; i++){
        if( clients[i]!= NULL){
            msgbuf stop;
            stop.mtype = STOP;
            stop.client_id = i;
            stop.other_id = i;
            strcpy(stop.content, "");

            send(stop);
        }
    }
    mq_close(s_queue);
    mq_unlink(SERVER);
    exit(0);
}
void send(msgbuf msg){
    char* command;
    sprintf(command,"%s %d %s\n",to_string(msg.mtype),msg.client_id,msg.content);
    mqd_t receiver = mq_open(clients[msg.other_id],O_RDWR);
    mq_send(receiver, command, MSG_SIZE,0);
    mq_close(receiver);

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
void save_log(msgbuf msg) {
    FILE *log_file = fopen("logs.txt", "a");

    switch (msg.mtype) {
        case INIT:
            if (msg.client_id == -1) {
                fprintf(log_file, "(INIT) Max number of clients is reached!\n");
            } else {
                fprintf(log_file, "(INIT) Client ID: %d\n", msg.client_id);
            }
            break;
        case LIST:
            fprintf(log_file, "(LIST) Client ID: %d\n", msg.client_id);
            break;
        case ALL:
            fprintf(log_file, "Message: %s\n", msg.content);
            fprintf(log_file, "(2ALL) Client ID: %d\n", msg.client_id);
            break;
        case ONE:
            fprintf(log_file, "Message: %s\n", msg.content);
            fprintf(log_file, "(2ONE) Sender ID: %d, Receiver ID %d\n", msg.client_id, msg.other_id);
            break;
        case STOP:
            fprintf(log_file, "(STOP) Client ID: %d\n", msg.client_id);
            break;
    }

    fclose(log_file);
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