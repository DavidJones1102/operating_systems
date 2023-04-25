#define BARBERS_SIZE 3
#define CLIENTS_SIZE 8
#define CHAIRS_SIZE 2
#define QUEUE_SIZE 10 
#define BARBERS 0
#define CLIENTS 1
#define CHAIRS 2
#define QUEUE 3
#define SEM_ID 1
#define SEM_SIZE 4
// #define BARBERS_ID 0
// #define CLIENTS_ID 1
// #define CHAIRS_ID 2
// #define QUEUE_ID 3 
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};