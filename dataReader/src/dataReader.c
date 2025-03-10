#include "dataStructures.h"
#include "logger.h"
#include "sharedMemory.h"
#include "messageQueue.h"
#include "dcManager.h"
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

volatile bool running = true;

void handleSignal(int sig)
{
    logMessage("Received termination signal. Shutting down..");
    running = false;
}

int main(void) {
    int msgQueueID;
    int shmID;
    MasterList* masterList;
    MachineStatusMessage incomingMessage;

    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    // Initialize message queue
    msgQueueID = initializeMessageQueue();
    if (msgQueueID == -1) {
        logMessage("Failed to initialize message queue. Exiting.");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory
    shmID = initializeSharedMemory(&masterList, msgQueueID);
    if (shmID == -1) {
        logMessage("Failed to initialize shared memory. Exiting.");
        cleanupMessageQueue(msgQueueID);
        exit(EXIT_FAILURE);
    }

    logMessage("DC Monitor started successfully");
    sleep(15); // Allow time for DCs to start

    // Main monitoring loop
    while (running) {
        
        checkInactiveMachines(masterList, shmID, msgQueueID);

        // Receive and process messages
        if (receiveMessage(masterList->msgQueueID, &incomingMessage) == 0) {
            updateDCStatus(masterList, incomingMessage.machineID, incomingMessage.statusCode, incomingMessage.statusMessage);
        }

        sleep(1.5);
    }
    
    cleanup(shmID, msgQueueID, masterList);
    return 0; 
}