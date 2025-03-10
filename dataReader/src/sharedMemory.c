/*
* FILE : sharedMemory.c
* PROJECT : Hoochamacallit System
* PROGRAMMERS : Josh Horsley,
* FIRST VERSION : 2025-03-09
* UPDATED : 2025-03-09
* DESCRIPTION :
* This file contains functions for initializing and cleaning up shared memory 
* in the Data Reader (DR) application. The `initializeSharedMemory` function sets up the shared 
* memory segment, attaches it, and initializes it with default values. If the segment doesn't exist, 
* it is created. The `cleanupSharedMemory` function detaches and removes the shared memory 
* segment when it is no longer needed. Logging is used throughout the functions 
* to track any errors or significant actions.
* 
* FUNCTIONS:
* - initializeSharedMemory : Initializes shared memory, creates and attaches it if necessary.
* - cleanupSharedMemory : Detaches and removes the shared memory segment.
*
*/

#include "sharedMemory.h"
#include "logger.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

//
// FUNCTION : initializeSharedMemory
// DESCRIPTION : 
// Initializes shared memory by generating a key and checking for its existence. 
// If the memory doesn't exist, it creates and initializes a new segment with the 
// provided message queue ID. If successful, the shared memory ID is returned, 
// otherwise, -1 is returned.
// PARAMETERS :
// MasterList** masterListPtr : Pointer to the MasterList structure to be initialized.
// int msgQueueID : The message queue ID to associate with the shared memory.
// RETURNS :
// int : The shared memory ID on success, or -1 if an error occurs.
//

int initializeSharedMemory(MasterList** masterListPtr, int msgQueueID) {
    key_t shmKey;
    int shmID;
    MasterList* masterList;

    shmKey = ftok(".", SHM_KEY);
    if (shmKey == -1) {
        logMessage("Error: Failed to generate shared memory key");
        return -1;
    }

    // Check if shared memory exists
    if ((shmID = shmget(shmKey, sizeof(MasterList), 0)) == -1) {
        // memory doesn't exist - create it
        shmID = shmget(shmKey, sizeof(MasterList), (IPC_CREAT | 0660));
        if (shmID == -1) {
            logMessage("Shared memory creation failed");
            return -1;
        }

        // Attach the shared memory segment
        masterList = (MasterList*)shmat(shmID, NULL, 0);
        if (masterList == (void*)-1) {
            logMessage("Failed to attach shared memory");
            return -1;
        }

        // Initialize the shared memory
        masterList->msgQueueID = msgQueueID;
        masterList->numberOfDCs = 0;
        memset(masterList->dc, 0, sizeof(masterList->dc));
        logMessage("Shared memory created and initialized");
    } else {
        // Attach the shared memory segment
        masterList = (MasterList*)shmat(shmID, NULL, 0);
        if (masterList == (void*)-1) {
            logMessage("Failed to attach shared memory");
            return -1;
        }
        logMessage("Shared memory attached to existing segment");
    }

    *masterListPtr = masterList;
    return shmID;
}

//
// FUNCTION : cleanupSharedMemory
// DESCRIPTION : 
// Detaches the shared memory segment associated with the provided pointer. If the detachment 
// operation fails, an error message is logged. This function is used for cleaning up the 
// shared memory once it is no longer needed.
// PARAMETERS :
// int shmID : The shared memory ID used for identifying the shared memory segment.
// MasterList* masterList : The pointer to the shared memory segment to be detached.
// RETURNS :
// void.
//

void cleanupSharedMemory(int shmID, MasterList* masterList) {
    // Detach shared memory
    if (shmdt(masterList) == -1) {
        logMessage("Error: Failed to detach shared memory");
    }

    // Remove shared memory
    if (shmctl(shmID, IPC_RMID, NULL) == -1) {
        logMessage("Error: Failed to remove shared memory");
    }
}