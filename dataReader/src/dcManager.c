#include "dcManager.h"
#include "logger.h"
#include "sharedMemory.h"
#include "messageQueue.h"
#include <string.h>
#include <unistd.h>

void updateDCStatus(MasterList* masterList, pid_t machineID, int statusCode, const char* statusMessage) {
    int found = 0;
    for (int i = 0; i < masterList->numberOfDCs; i++) {
        if (masterList->dc[i].dcProcessID == machineID) {
            masterList->dc[i].lastTimeHeardFrom = time(NULL);

            if (statusCode == 6) {
                char logMsg[256];
                snprintf(logMsg, sizeof(logMsg), "DC-%02d [%d] has gone OFFLINE. Removing from master list.", 
                         i + 1, machineID);
                logMessage(logMsg);
                removeMachineFromList(masterList, i, 0, 0); // shmID and msgQueueID will be provided in the actual cleanup call
            } else {
                char logMsg[256];
                snprintf(logMsg, sizeof(logMsg), "DC-%02d [%d] updated in master list MSG RECEIVED - Status: %d (%s)", 
                         i + 1, machineID, statusCode, statusMessage);
                logMessage(logMsg);
            }
            found = 1;
            break;
        }
    }
    
    if (!found) {
        if (masterList->numberOfDCs < MAX_DC_ROLES) {
            masterList->dc[masterList->numberOfDCs].dcProcessID = machineID;
            masterList->dc[masterList->numberOfDCs].lastTimeHeardFrom = time(NULL);
            
            char logMsg[256];
            snprintf(logMsg, sizeof(logMsg), "DC-%02d [%d] added to the master list - NEW DC - Status: 0 (Everything is OKAY)", 
                     masterList->numberOfDCs + 1, machineID);
            logMessage(logMsg);
            
            masterList->numberOfDCs++;
        } else {
            logMessage("Master list is full, Cannot add more machines.");
        }
    }
}

void removeMachineFromList(MasterList* masterList, int index, int shmID, int msgQueueID) {
    for (int i = index; i < masterList->numberOfDCs - 1; i++) {
        masterList->dc[i] = masterList->dc[i+1];
    }
    masterList->numberOfDCs--;

    if (masterList->numberOfDCs == 0 && shmID != 0 && msgQueueID != 0) {
        logMessage("No active DCs remaining. Shutting down the system.");
        cleanup(shmID, msgQueueID, masterList);
    }
}

void checkInactiveMachines(MasterList* masterList, int shmID, int msgQueueID) {
    time_t currentTime = time(NULL);

    for (int i = 0; i < masterList->numberOfDCs; i++) {
        if (difftime(currentTime, masterList->dc[i].lastTimeHeardFrom) > INACTIVE_THRESHOLD) {
            char logMsg[256];
            snprintf(logMsg, sizeof(logMsg), "DC-%02d [%d] removed from master list - NON-RESPONSIVE", 
                     i + 1, masterList->dc[i].dcProcessID);
            logMessage(logMsg);

            removeMachineFromList(masterList, i, shmID, msgQueueID);
            i--; // Adjust index since we removed an element
        }
    }
}

void cleanup(int shmID, int msgQueueID, MasterList* masterList) {
    cleanupSharedMemory(shmID, masterList);
    cleanupMessageQueue(msgQueueID);
    
    logMessage("Cleanup complete. Exiting...");
    exit(EXIT_SUCCESS);
}