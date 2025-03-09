#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <time.h> 
#include <string.h>

#define MAX_DC_ROLES 10
#define SHM_KEY 16535
#define MSG_KEY 1234 //temporary key number

typedef struct 
{
    pid_t dcProcessID;
    time_t lastTimeHeardFrom;

} DCInfo;

typedef struct
{
    int msgQueueID;
    intnumverOfDCs;
    DCInfo dc[MAX_DC_ROLES];

} MasterList;

int main(void)
{

}

