#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include<string.h>
#include<time.h>

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