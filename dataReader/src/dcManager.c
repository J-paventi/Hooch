/*
* FILE : dcManager.c
* PROJECT : Hoochamacallit System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon, Tony
* FIRST VERSION : 2025-03-09
* UPDATED : 2025-03-09
* DESCRIPTION :
* This file contains the core functionality for managing and monitoring data collectors (DCs) 
* in the DR application. It handles updating DC statuses, adding new machines to the master list, 
* removing inactive or offline machines, and performing system cleanup when no machines remain online.
* 
* FUNCTIONS:
* - updateDCStatus : Updates a machine’s status in the master list or adds it if not present.
* - removeMachineFromList : Removes a machine from the master list and triggers cleanup if no DCs remain.
* - checkInactiveMachines : Checks for unresponsive machines and removes them after a timeout.
* - cleanup : Cleans up shared memory and message queues, then gracefully exits the program.
*
*/

#include "dcManager.h"
#include "logger.h"
#include "sharedMemory.h"
#include "messageQueue.h"
#include <string.h>
#include <unistd.h>

// FUNCTION : updateDCStatus
// DESCRIPTION : 
// Updates the status of a data collector (DC) in the master list. If the DC is already present, 
// it updates the last communication timestamp and logs the status message. If the status code indicates 
// the DC has gone offline, it removes the DC from the master list. If the DC is not found and the list 
// is not full, it adds the DC as a new entry with an initial status message.
// PARAMETERS :
// MasterList* masterList : Pointer to the master list of data collectors.
// pid_t machineID : The process ID of the data collector.
// int statusCode : The status code received from the data collector.
// const char* statusMessage : A descriptive status message from the data collector.
// RETURNS :
// void.
//

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

// FUNCTION : removeMachineFromList
// DESCRIPTION : 
// Removes a data collector (DC) from the master list by shifting the remaining entries down. 
// Decrements the total number of DCs in the list. If no DCs remain and valid shared memory 
// and message queue IDs are provided, it triggers the system cleanup process.
// PARAMETERS :
// MasterList* masterList : Pointer to the master list of data collectors.
// int index : The index of the DC to be removed from the list.
// int shmID : The ID of the shared memory segment (used for cleanup).
// int msgQueueID : The ID of the message queue (used for cleanup).
// RETURNS :
// void.
//

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

// FUNCTION : checkInactiveMachines
// DESCRIPTION : 
// Scans the master list of data collectors to identify and remove inactive machines. 
// A machine is considered inactive if the elapsed time since the last communication 
// exceeds 35 seconds. If an inactive machine is found, it is removed 
// from the list, and a log message is generated. The function also adjusts the loop index 
// to handle element shifts after removal.
// PARAMETERS :
// MasterList* masterList : Pointer to the master list of data collectors.
// int shmID : The ID of the shared memory segment (used for cleanup if no DCs remain).
// int msgQueueID : The ID of the message queue (used for cleanup if no DCs remain).
// RETURNS :
// void.
//

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

// FUNCTION : cleanup
// DESCRIPTION : 
// Performs cleanup operations for the system by deallocating shared memory, 
// destroying the message queue, and logging the cleanup process. Once cleanup 
// is complete, the function terminates the program with a success exit code.
// PARAMETERS :
// int shmID : The ID of the shared memory segment to be released.
// int msgQueueID : The ID of the message queue to be destroyed.
// MasterList* masterList : Pointer to the master list of data collectors (used for memory cleanup).
// RETURNS :
// void.
//

void cleanup(int shmID, int msgQueueID, MasterList* masterList) {
    cleanupSharedMemory(shmID, masterList);
    cleanupMessageQueue(msgQueueID);
    
    logMessage("Cleanup complete. Exiting...");
    exit(EXIT_SUCCESS);
}