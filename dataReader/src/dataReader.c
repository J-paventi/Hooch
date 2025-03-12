/*
* FILE : dataReader.c
* PROJECT : Hoochamacallit System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-09
* UPDATED : 2025-03-09
* DESCRIPTION :
* This file contains the main logic for the DC Monitor in the Data Reader (DR) application. 
* The program initializes the message queue and shared memory, sets up signal handling for termination,
* and enters a monitoring loop where it periodically checks for inactive machines and processes incoming 
* messages. The DC Monitor continues to run until a termination signal is received. 
* Afterward, it cleans up resources such as shared memory and the message queue.
*
* FUNCTIONS:
* - handleSignal : Handles termination signals (SIGINT, SIGTERM) to stop the monitoring loop.
* - main : The main monitoring loop that initializes resources, processes messages, and checks for inactive machines.
*
*/

#include "dataStructures.h"
#include "logger.h"
#include "sharedMemory.h"
#include "messageQueue.h"
#include "dcManager.h"
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

volatile bool running = true;

//
// FUNCTION : handleSignal
// DESCRIPTION : 
// Handles termination signals (SIGINT, SIGTERM). When a termination signal is received, 
// it logs a shutdown message and sets the global 'running' flag to false, causing the 
// main loop to exit and gracefully shut down the program.
// PARAMETERS :
// int sig : The signal number that triggered the handler.
// RETURNS :
// void.
//

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
        masterList->terminateDC = true;
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory
    shmID = initializeSharedMemory(&masterList, msgQueueID);
    if (shmID == -1) {
        logMessage("Failed to initialize shared memory. Exiting.");
        cleanupMessageQueue(msgQueueID);
        masterList->terminateDC = true;
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