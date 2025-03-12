#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "logger.h"
#include "data_structures.h"         // for logging functions

// Constants
#define MAX_RETRIES 100
#define RETRY_SLEEP 10
#define WOD_ACTIONS 21

// Function prototypes
bool generateKey(key_t *shmKey);
bool getSharedMemoryID(key_t shmKey, int *shmID);
void DX_MainLoop(MasterList *masterList, int shmID);
void randomSleep();
int wheelOfDestruction(MasterList *masterList);
bool killDC(MasterList *masterList, int index);
bool deleteMessageQueue();

int main()
{
    key_t shmKey;          // Shared memory key
    int shmID;             // Shared memory ID
    MasterList *masterList; // Pointer to shared memory

    int retryCount = 0;

    logMessage("---DX Data Corruption Process Started---");

    // Step 1: Generate the shared memory key
    if (!generateKey(&shmKey))
    {
        logMessage("ERROR: Failed to generate shared memory key");
        return -1;
    }

    // Step 2: Get the shared memory ID (retry up to MAX_RETRIES times)
    while (!getSharedMemoryID(shmKey, &shmID))
    {
        if (retryCount >= MAX_RETRIES)
        {
            logMessage("ERROR: Failed to attach to shared memory after 100 retries");
            return -1;
        }
        retryCount++;
        sleep(RETRY_SLEEP);
    }

    // Step 3: Attach to the shared memory
    masterList = (MasterList *)shmat(shmID, NULL, 0);
    if (masterList == (void *)-1)
    {
        logMessage("ERROR: Failed to attach to shared memory");
        return -1;
    }

    // Step 4: Enter the main processing loop
    DX_MainLoop(masterList, shmID);

    // Step 5: Detach from shared memory
    if (shmdt(masterList) == -1)
    {
        logMessage("ERROR: Failed to detach from shared memory");
        return -1;
    }

    logMessage("---DX Data Corruption Process Ended---");
    return 0;
}

// Main processing loop for DX
void DX_MainLoop(MasterList *masterList, int shmID)
{


    key_t msgKey = ftok("/tmp/keyfile", MSG_KEY);
    if (msgKey == -1)
    {
        logMessage("ERROR: Failed to generate message queue key");
        return;
    }


    while (true)
    {
        // Step 1: Sleep for a random amount of time (10-30 seconds)
        randomSleep();



        logMessage("DX: Checking for message queue");
        // Step 2: Check for the existence of the message queue
        int msgQueueId = msgget(msgKey, IPC_EXCL | 0666);







        //       msgid = msgget(key, IPC_EXCL | 0666);
        if (msgQueueId == -1)
        {
            logMessage("DX detected that msgQ is gone – assuming DR/DCs done");
            break; // Exit the loop
        }

        logMessage("DX: Message queue found");

        // Step 3: Select and execute an action from the Wheel of Destruction
        int action = wheelOfDestruction(masterList);

        logMessage("DX: Executed action from Wheel of Destruction");

        char logMsg[100];
        sprintf(logMsg, "Wheel of Destruction Action: %d", action);
        logMessage(logMsg);
    }
}

// Generate a shared memory key using ftok
bool generateKey(key_t *shmKey)
{
    *shmKey = ftok(".", SHM_KEY);
    if (*shmKey == -1)
    {
        logMessage("ERROR: Failed to generate key with ftok");
        return false;
    }
    return true;
}

// Get the shared memory ID using shmget
bool getSharedMemoryID(key_t shmKey, int *shmID)
{
    *shmID = shmget(shmKey, sizeof(MasterList), 0666);
    if (*shmID == -1)
    {
        logMessage("ERROR: Failed to get shared memory ID with shmget");
        return false;
    }
    return true;
}

// Sleep for a random amount of time (10-30 seconds)
void randomSleep()
{
    int sleepTime = 10 + (rand() % 21); // Random time between 10 and 30 seconds
    sleep(sleepTime);
}

// Select an action from the Wheel of Destruction
int wheelOfDestruction(MasterList *masterList)
{
    int action = rand() % WOD_ACTIONS; // Random number between 0 and 20

    switch (action)
    {
        case 1: case 4: case 11:
            killDC(masterList, 0); // Kill DC-01
            break;
        case 2: case 5: case 15:
            killDC(masterList, 2); // Kill DC-03
            break;
        case 3: case 6: case 13:
            killDC(masterList, 1); // Kill DC-02
            break;
        case 7:
            killDC(masterList, 3); // Kill DC-04
            break;
        case 9:
            killDC(masterList, 4); // Kill DC-05
            break;
        case 10: case 17:
            deleteMessageQueue(); // Delete the message queue
            break;
        case 12:
            killDC(masterList, 5); // Kill DC-06
            break;
        case 14:
            killDC(masterList, 6); // Kill DC-07
            break;
        case 16:
            killDC(masterList, 7); // Kill DC-08
            break;
        case 18:
            killDC(masterList, 8); // Kill DC-09
            break;
        case 20:
            killDC(masterList, 9); // Kill DC-10
            break;
        default:
            // Do nothing for cases 0, 8, 19
            break;
    }

    return action;
}

// Kill a DC process by sending a SIGHUP signal
bool killDC(MasterList *masterList, int index)
{
    if (index < 0 || index >= masterList->numberOfDCs)
    {
        char logMsg[100];
        sprintf(logMsg, "ERROR: Invalid DC index %d", index);
        logMessage(logMsg);
        return false;
    }

    pid_t dcPid = masterList->dc[index].dcProcessID; // Use dcProcessID instead of pid
    if (kill(dcPid, SIGHUP) == -1)
    {
        char logMsg[100];
        sprintf(logMsg, "ERROR: Failed to kill DC process %d", dcPid);
        logMessage(logMsg);
        return false;
    }

    char logMsg[100];
    sprintf(logMsg, "Successfully killed DC process %d", dcPid);
    logMessage(logMsg);
    return true;
}

// Delete the message queue
bool deleteMessageQueue()
{
    key_t msgKey = ftok("/tmp/keyfile", MSG_KEY);
    if (msgKey == -1)
    {
        logMessage("ERROR: Failed to generate message queue key");
        return false;
    }

    int msgQueueId = msgget(msgKey, 0666);
    if (msgQueueId == -1)
    {
        logMessage("ERROR: Failed to get message queue ID");
        return false;
    }

    if (msgctl(msgQueueId, IPC_RMID, NULL) == -1)
    {
        logMessage("ERROR: Failed to delete message queue");
        return false;
    }

    logMessage("Successfully deleted message queue");
    return true;
}