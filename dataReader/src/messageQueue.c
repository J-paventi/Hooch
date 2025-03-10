/*
* FILE : messageQueue.c
* PROJECT : Hoochamacallit System
* PROGRAMMERS : Josh Horsley, 
* FIRST VERSION : 2025-03-09
* UPDATED : 2025-03-09
* DESCRIPTION :
* This file contains functions for managing the message queue in the Data Reader (DR) application. 
* The `initializeMessageQueue` function initializes the message queue, checking for its existence 
* and creating it if necessary. The `receiveMessage` function receives messages from the queue, 
* and the `cleanupMessageQueue` function removes the message queue once it is no longer needed. 
* These functions ensure that the message queue is properly handled throughout the application.
*
* FUNCTIONS:
* - initializeMessageQueue : Initializes and creates a message queue if necessary, or attaches to an existing one.
* - receiveMessage : Receives a message from the message queue and stores it in the provided structure.
* - cleanupMessageQueue : Removes the specified message queue.
*
*/

#include "messageQueue.h"
#include "logger.h"
#include <sys/msg.h>
#include <sys/ipc.h>

//
// FUNCTION : initializeMessageQueue
// DESCRIPTION : 
// This function attempts to initialize a message queue by generating a unique key 
// using ftok and then checking if the message queue already exists. If it doesn't exist, 
// a new message queue is created with the specified permissions. If the message queue 
// is successfully created or attached, the function returns the message queue ID. 
// If any errors occur, appropriate error messages are logged, and the function returns -1.
// PARAMETERS :
// None
// RETURNS :
// int : The message queue ID on success, or -1 if an error occurs.
//

int initializeMessageQueue() {
    key_t msgKey;
    int msgQueueID;

    msgKey = ftok(".", MSG_KEY);
    if (msgKey == -1) {
        logMessage("Error: Failed to generate message queue key");
        return -1;
    }

    // Check if the message queue exists
    if ((msgQueueID = msgget(msgKey, 0)) == -1) {
        // Message queue doesn't exist so create it
        msgQueueID = msgget(msgKey, (IPC_CREAT | 0660));
        if (msgQueueID == -1) {
            logMessage("Error: message queue creation failed");
            return -1;
        } else {
            logMessage("Message queue created");
        }
    } else {
        logMessage("Attached to existing message queue");
    }

    return msgQueueID;
}

//
// FUNCTION : receiveMessage
// DESCRIPTION : 
// Receives a message from the specified message queue using msgrcv. The function retrieves 
// the message into the provided MachineStatusMessage structure, excluding the message type 
// from the size calculation. If the message cannot be received, an error message is logged 
// and the function returns -1. If the message is successfully received, it returns 0.
// PARAMETERS :
// int queueID : The ID of the message queue from which to receive the message.
// MachineStatusMessage* message : Pointer to the structure where the received message will be stored.
// RETURNS :
// int : 0 if the message is successfully received, or -1 if an error occurs.
//

int receiveMessage(int queueID, MachineStatusMessage* message) {
    if (msgrcv(queueID, message, sizeof(*message) - sizeof(long), 0, 0) == -1) {
        logMessage("msgrcv failed");
        return -1;
    }
    return 0;
}

//
// FUNCTION : cleanupMessageQueue
// DESCRIPTION : 
// Removes the specified message queue using the msgctl function with the IPC_RMID command. 
// If the removal fails, an error message is logged. This function does not return any value.
// PARAMETERS :
// int msgQueueID : The ID of the message queue to be removed.
// RETURNS :
// void : This function does not return a value. It performs the cleanup operation and logs any errors.
//

void cleanupMessageQueue(int msgQueueID) {
    if (msgctl(msgQueueID, IPC_RMID, NULL) == -1) {
        logMessage("Failed to remove message queue");
    }
}