#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include<string.h>

#define SERVER_ID 1
#define CLIENTS 5
#define MAX_LENGTH 512


typedef struct msgbuf
{
    long mtype;
    key_t key;
    int client_id;
    // int other_id;
    char content[MAX_LENGTH];
} msgbuf;

typedef enum cmd{
    INIT=1,
    LIST,
    ONE,
    ALL,
    STOP,
    INVALID    
} cmd;
