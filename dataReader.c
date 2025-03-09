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
    key_t msgKey;
    key_t shmKey;
    int msgQueueID;
    int shmID;
    MasterList* masterList;

    msgKey = ftok(".", MSG_KEY);
    shmKey = ftok(".", SHM_KEY);

    if (msgKey == -1 || shmKey == -1)
    {
        perror("Failed to genrate keys"); //remove later
        exit(EXIT_FAILURE);
    }

    // check if the message queue exists...
    if((msgQueueID = msgget(msgKey, 0)) == -1)
    {
        // message queue doesn't exist so create it
        msgQueueID = msgget(msgKey, (IPC_CREAT | 0660 ));
        if(msgQueueID == -1)
        {
            perror("message queue creation failed"); // remove later
        }else
        {
            printf("Message queue created with ID: %d \n", msgQueueID); // remove later
        }
    }

    //Check if enough shared memory exists and potentially allocates it
    if((shmID = shmget(shmKey, sizeof(MasterList),0)) == -1)
    {
        // memory doesnt exist - create it
        shmID = shmget(shmKey, sizeof(MasterList), (IPC_CREAT |0660));
        if (shmID == -1)
        {
            perror("Shared memory creation failed"); //remove later
            exit(EXIT_FAILURE);
        }
    }

    masterList = (MasterList*)shmat(shmID, NULL, 0):
    if (masterList == (void*)-1)
    {
        perror("failed to attach shared memory";)
        exit(EXIT_FAILURE);
    }
}

