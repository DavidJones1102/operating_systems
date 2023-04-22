#include<unistd.h>
#include "common.h"
int s_id;
key_t clients[CLIENTS];
int next_client = 0;
// list inny content
// error handling
void handler(int sig_n){
    printf("handler :)\n");
    if( msgctl(s_id,IPC_RMID,NULL) == -1){
        perror("msgctl error");
    };
    exit(0);
}
void handle_init(msgbuf msg);
void handle_list(msgbuf msg);
void handle_all(msgbuf msg);
void handle_one(msgbuf msg);
void handle_stop(msgbuf msg);

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

    signal(SIGINT, handler);

    for( int i = 0; i < CLIENTS; i++){
        clients[i] = -1;
    }
    msgbuf msg;

    // struct msqid_ds msglist;
    // int num_msg = 0;

    while (1)
    {
        // msgctl(s_id,IPC_STAT,&msglist);                      //sense the message queue
  	    // num_msg = msglist.msg_qnum;		
        // // if (num_msg == 0) continue;
       
        msgrcv(s_id, (void *) &msg, sizeof(msgbuf),-6, 0);
        // sleep(5);
        switch (msg.mtype) {
            case INIT:
                handle_init(msg);
                break;
            case LIST:
                handle_list(msg);
                // store_message_logs(msg);
                break;
            case ALL:
                handle_all(msg);
                // store_message_logs(msg);
                break;
            case ONE:
                handle_one(msg);
                // store_message_logs(msg);
                break;
            case STOP:
                handle_stop(msg);
                // store_message_logs(msg);
                break;
            default:
                printf("Unexpected message type!\n");
        }
    }
    
    return 0;
}

void handle_init(msgbuf msg){
    printf("INIT \n");
    if( next_client < CLIENTS){
        msg.client_id = next_client; //yyy??
        clients[next_client] = msg.key;
        next_client++;
        printf("%d",msg.key);
    }
    else{
        msg.client_id = -1;
        printf("%d",-1);
    }
    fflush(NULL);
    int c_id = msgget(msg.key, 0);
    msgsnd(c_id, (void *) &msg, sizeof(msgbuf), 0);
    // printf("%d\n",msg.key);

    
}
void handle_list(msgbuf msg){
    printf("LIST\n");
    int c_id = msgget(clients[msg.client_id], 0); 
    msgbuf to_send;
    strcpy(to_send.content, "Cooonnnttteeennntt \n");
    msgsnd(c_id,&to_send, sizeof(msgbuf), 0);
    
}
void handle_all(msgbuf msg){
    printf("2ALL\n");
    for(int i=0; i<CLIENTS; i++){
        if(clients[i]!=-1 && i!=msg.client_id){
            printf("hinallly %d\n",clients[i]);
            int other_id = msgget(clients[i],0);
            msgsnd(other_id, &msg, sizeof(msgbuf),0);
        }
    }
}
void handle_one(msgbuf msg){
    // if(clients[msg.other_id] != -1){
    //     int c_id = msgget(clients[msg.other_id],0);
    //     msgsnd(c_id, &msg, sizeof(msgbuf),0);
    // }
}
void handle_stop(msgbuf msg){
    
}