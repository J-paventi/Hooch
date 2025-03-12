#include "shared_memory.h"
#include "logger.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

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
