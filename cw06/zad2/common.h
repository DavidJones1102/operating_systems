#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include<mqueue.h>
#include <fcntl.h>
#include<sys/stat.h>
#include <signal.h>
#include<string.h>
#include<time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define SERVER_ID 1
#define CLIENTS 5
#define MAX_LENGTH 255



typedef struct msgbuf
{
    long mtype;
    key_t key;
    int client_id;
    int other_id;
    struct tm time;
    char content[MAX_LENGTH];
} msgbuf;

typedef enum cmd{
    INIT=1,
    ONE,
    ALL,
    STOP,
    LIST,
    INVALID    
} cmd;

const int MSG_SIZE = sizeof(msgbuf)-sizeof(long);

// mqd_t create_queue(const char* name) {
//     struct mq_attr attr;
//     attr.mq_maxmsg = CLIENTS;
//     attr.mq_msgsize = MSG_SIZE;
//     return mq_open(name, O_RDWR | O_CREAT, 0666, &attr);
// }