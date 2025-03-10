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
#define INACTIVE_THRESHOLD 35 //seconds
#define LOG_FILE "tmp/dataMonitor.log"

void logMessage(const char* message);
void removeMachineFromList(MasterList* masterList, int index, int shmID, int msgQueueID);
void checkInactiveMachines(MasterList* masterList, int smhID, int msgQueueID);
void cleanup(int shmID, int msgQueueID, MasterList* masterList);

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
        logMessage("Error: Failed to generate keys"); //remove later
        exit(EXIT_FAILURE);
    }

    // check if the message queue exists...
    if((msgQueueID = msgget(msgKey, 0)) == -1)
    {
        // message queue doesn't exist so create it
        msgQueueID = msgget(msgKey, (IPC_CREAT | 0660 ));
        if(msgQueueID == -1)
        {
            logMessage("Error: message queue creation failed"); // remove later
            exit(EXIT_FAILURE);
        }else
        {
            logMessage("Message queue created"); // remove later
        }
    }

    //Check if enough shared memory exists and potentially allocates it
    if ((shmID = shmget(shmKey, sizeof(MasterList), 0)) == -1)
{
    // memory doesn't exist - create it
    shmID = shmget(shmKey, sizeof(MasterList), (IPC_CREAT | 0660));
    if (shmID == -1)
    {
        logMessage("Shared memory creation failed");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment
    masterList = (MasterList*)shmat(shmID, NULL, 0);
    if (masterList == (void*)-1)
    {
        logMessage("Failed to attach shared memory");
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
        logMessage("Failed to attach shared memory");
        exit(EXIT_FAILURE);
    }
    logMessage("shared memory attached");
}

    sleep(15); // allow time for DCs to Start

    MachineStatusMessage incomingMessage;

    while(1)
    {
        checkInactiveMachines(masterList, shmID, msgQueueID);

        if (msgrcv(masterList->msgQueueID, &incomingMessage, sizeof(incomingMessage) - sizeof(long), 0, 0) == -1)
        {
            logMessage("msgrcv failed"); // remove later
            continue;
        }

        int machineID = incomingMessage.machineID;
        int statusCode = incomingMessage.statusCode;

        //check if this machine is already in master list
        int found = 0;
        for (int i = 0; i < masterList->numberOfDCs; i++)
        {
            if(masterList->dc[i].dcProcessID == machineID) 
            {
                masterList->dc[i].lastTimeHeardFrom = time(NULL);

                if (statusCode == 6)
                {
                    char logMsg[256];
                    snprintf(logMsg, sizeof(logMsg), "DC-%02d [%d] has gone OFFLINE. Removing from master list. \n", i + 1, machineID);
                    logMessage(logMsg);
                    removeMachineFromList(masterList, i, shmID, msgQueueID);
                }
                else
                {
                    char logMsg[256];
                    snprintf(logMsg, sizeof(logMsg), "DC-%02d [%d] updated in master list MSG RECEIVED - Status: %d (%s)\n", i + 1, machineID, statusCode, incomingMessage.statusMessage);
                    logMessage(logMsg);
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

                char logMsg[256];
                snprintf(logMsg, sizeof(logMsg), "DC-%02d [%d] added to the master list - NEW DC - Status: 0 (Everything is OKAY)\n", masterList->numberOfDCs, machineID);
                logMessage(logMsg);
            }
            else
            {
                logMessage("Master list is full, Cannot add more machines.\n");
            }
        }

        sleep(1.5);
    }
    
    return 0;

}

void removeMachineFromList(MasterList* masterList, int index, int shmID, int msgQueueID)
    {
        for (int i =index; i < masterList->numberOfDCs - 1; i++)
        {
            masterList->dc[i] = masterList->dc[i+1];
        }
        masterList->numberOfDCs--;

        if (masterList->numberOfDCs == 0)
        {
            logMessage("No active DCs remaining. Shutting down the system.\n");
            cleanup(shmID, msgQueueID, masterList);
        }
    }


void checkInactiveMachines(MasterList* masterList, int smhID, int msgQueueID)
{
    time_t currentTime = time(NULL);

    for (int i = 0; i < masterList->numberOfDCs; i++)
    {
        if (difftime(currentTime, masterList->dc[i].lastTimeHeardFrom) > INACTIVE_THRESHOLD)
        {
            char logMsg[256];
            snprintf(logMsg, sizeof(logMsg), "DC-%02d [%d] removed from master list - NON-RESPONSIVE", i + 1, masterList->dc[i].dcProcessID);
            logMessage(logMsg);

            removeMachineFromList(masterList, i, smhID, msgQueueID);
            i--;
        }
    }
}

void cleanup(int shmID, int msgQueueID, MasterList* masterList)
{
    //Detach shared memory
    if (shmdt(masterList) == -1)
    {
        logMessage("Error: Failed to detach shared memory"); // replace wit log
    }

    if (shmctl(shmID, IPC_RMID, NULL) == -1)
    {
        logMessage("Error: Failed to remove shared memory"); // replace wit log
    }

    if(msgctl(msgQueueID, IPC_RMID, NULL) == -1)
    {
        logMessage("Failed to remove message queue"); // replace wit log
    }
    
   logMessage ("Cleanup complete. Exiting...\n"); //remove later
    exit(EXIT_SUCCESS);
}

void logMessage(const char* message)
{
    FILE* logFile = fopen(LOG_FILE, "a");
    if (logFile) 
    {
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

        fprintf(logFile, "[%s] : %s\n", timestamp, message);
        fclose(logFile);
    }
}