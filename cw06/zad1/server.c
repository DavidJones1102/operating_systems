#include<unistd.h>
#include "common.h"
int s_id;
key_t clients[CLIENTS];
int next_client = 0;
// error handling

void handle_init(msgbuf msg);
void handle_list(msgbuf msg);
void handle_all(msgbuf msg);
void handle_one(msgbuf msg);
void handle_stop(msgbuf msg);
void SIGINT_handler(int n);
void save_log(msgbuf msg);

int main(){

    key_t key;
    if((key = ftok(getenv("HOME"), SERVER_ID)) == -1){
        perror("ftok error");
        exit(1);
    }

    if((s_id = msgget(key, IPC_CREAT | 0666)) == -1) { // Q_PERM
        perror("msget error");
        exit(1);
    }
    // printf("%d \n", s_id);

    signal(SIGINT, SIGINT_handler);

    for( int i = 0; i < CLIENTS; i++){
        clients[i] = -1;
    }
    msgbuf msg;

    // struct msqid_ds msglist;
    // int num_msg = 0;

    while (1)
    {
    
        msgrcv(s_id, (void *) &msg, MSG_SIZE,-6, 0);

        switch (msg.mtype) {
            case INIT:
                handle_init(msg);
                break;
            case LIST:
                handle_list(msg);
             
                break;
            case ALL:
                handle_all(msg);
             
                break;
            case ONE:
                handle_one(msg);
             
                break;
            case STOP:
                handle_stop( msg );
             
                break;
            default:
                printf("Unexpected message type!\n");
        }
    }
    
    return 0;
}

void handle_init(msgbuf msg){
    printf("INIT \n");
    while (clients[next_client] != -1 && next_client<CLIENTS)
    {
        next_client++;
    }
    
    if( next_client < CLIENTS){
        msg.client_id = next_client; 
        clients[next_client] = msg.key;
        next_client++;
    }
    else{
        msg.client_id = -1;
    }
    fflush(NULL);
    int c_id = msgget(msg.key, 0);
    msgsnd(c_id, (void *) &msg, MSG_SIZE, 0);
    // printf("%d\n",msg.key);
    save_log(msg);
    
}
void handle_list(msgbuf msg){
    printf("LIST\n");
    int c_id = msgget(clients[msg.client_id], 0);
    char list[MAX_LENGTH]="";
    char temp[13];
    for(int i=0; i<CLIENTS; i++){
        if(clients[i]!=-1 && i!=msg.client_id){
            sprintf(temp,"%d ",i);
            strcat(list, temp);
        }
    }

    strcpy(msg.content, list);
    msgsnd(c_id,&msg, MSG_SIZE, 0);
    save_log(msg);
    
}
void handle_all(msgbuf msg){
    printf("2ALL\n");
    for(int i=0; i<CLIENTS; i++){
        if(clients[i]!=-1 && i!=msg.client_id){
            int other_id = msgget(clients[i],0);
            msgsnd(other_id, &msg, MSG_SIZE,0);
        }
        save_log(msg);
    }
}
void handle_one(msgbuf msg){
    printf("2ONE\n");
    if(clients[msg.other_id]!=-1 ){
        int other_id = msgget(clients[msg.other_id],0);
        msgsnd(other_id, &msg, MSG_SIZE,0);
    }

    save_log(msg);
}
void handle_stop( msgbuf msg){

    clients[msg.client_id] = -1;
    if( msg.client_id < next_client){
        next_client = msg.client_id;
    }
    save_log(msg);
}

void SIGINT_handler(int n){
    printf("handler :)\n");
    for(int i=0; i<CLIENTS; i++){
        if( clients[i]!=-1){
            msgbuf stop;
            stop.mtype = STOP;
            int id = msgget(clients[i],0);
            msgsnd(id,&stop,MSG_SIZE,0);
        }
    }
    if( msgctl(s_id,IPC_RMID,NULL) == -1){
        perror("msgctl error");
    };
    exit(0);
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

    fprintf(log_file, "sent at: %02d:%02d:%02d\n\n\n",
            msg.time.tm_hour,
            msg.time.tm_min,
            msg.time.tm_sec);

    fclose(log_file);
}
