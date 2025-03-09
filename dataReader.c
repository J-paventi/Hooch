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

void removeMachineFromList(MasterList* masterList, int index);

typedef struct 
{
    pid_t dcProcessID;
    time_t lastTimeHeardFrom;

} DCInfo;

typedef struct
{
    int msgQueueID;
    int numberOfDCs;
    DCInfo dc[MAX_DC_ROLES];

} MasterList;

typedef struct 
{
    long msgType;
    pid_t machineID;
    int statusCode;
    char statusMessage[256];

} MachineStatusMessage;

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
            exit(EXIT_FAILURE);
        }else
        {
            printf("Message queue created with ID: %d \n", msgQueueID); // remove later
        }
    }

    //Check if enough shared memory exists and potentially allocates it
    if ((shmID = shmget(shmKey, sizeof(MasterList), 0)) == -1)
{
    // memory doesn't exist - create it
    shmID = shmget(shmKey, sizeof(MasterList), (IPC_CREAT | 0660));
    if (shmID == -1)
    {
        perror("Shared memory creation failed");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment
    masterList = (MasterList*)shmat(shmID, NULL, 0);
    if (masterList == (void*)-1)
    {
        perror("Failed to attach shared memory");
        exit(EXIT_FAILURE);
    }

    // Initialize the shared memory
    masterList->msgQueueID = msgQueueID;
    masterList->numberOfDCs = 0;
    memset(masterList->dc, 0, sizeof(masterList->dc));
}
else
{
    // Attach the shared memory segment
    masterList = (MasterList*)shmat(shmID, NULL, 0);
    if (masterList == (void*)-1)
    {
        perror("Failed to attach shared memory");
        exit(EXIT_FAILURE);
    }
}

    sleep(15); // allow time for DCs to Start

    MachineStatusMessage incomingMessage;

    while(1)
    {
        if (msgrcv(masterList->msgQueueID, &incomingMessage, sizeof(incomingMessage) - sizeof(long), 0, 0) == -1)
        {
            perror("msgrcv failed"); // remove later
            continue;
        }

        int machineID = incomingMessage.machineID;
        int statusCode = incomingMessage.statusCode;

        //check if this machine is aleady in master list
        int found = 0;
        for (int i = 0; i < masterList->numberOfDCs; i++)
        {
            if(masterList->dc[i].dcProcessID == machineID) 
            {
                masterList->dc[i].lastTimeHeardFrom = time(NULL);

                if (statusCode == 6)
                {
                    printf("DC-%02d [%d] has gone OFFLINE. Removing from master list. \n", i + 1, machineID);
                    removeMachineFromList(masterList, i);
                }
                else
                {
                    printf("DC-%02d [%d] updated in master list MSG RECEVIED - Status: %d (%s)\n", i + 1, machineID, statusCode, incomingMessage.statusMessage);
                }
                found = 1;
                break;
            } 
        }
        
        if (!found) 
        {
            if (masterList->numberOfDCs < MAX_DC_ROLES) 
            {
                masterList->dc[masterList->numberOfDCs].dcProcessID = machineID;
                masterList->dc[masterList->numberOfDCs].lastTimeHeardFrom = time(NULL);
                masterList->numberOfDCs++;

                printf("DC-%02d [%d] added to the master list - NEW DC - Status: 0 (Everything is OKAY)\n", masterList->numberOfDCs, machineID);
            }
            else
            {
                printf("Master list is full, Cannot add more machines.\n");
            }
        }

        sleep(1);
    }

}

void removeMachineFromList(MasterList* masterList, int index)
    {
        for (int i =index; i < masterList->numberOfDCs - 1; i++)
        {
            masterList->dc[i] = masterList->dc[i+1];
        }
        masterList->numberOfDCs--;
    }


