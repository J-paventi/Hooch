#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "dataStructures.h"

// Initialize message queue
int initializeMessageQueue();

// Receive message from queue
int receiveMessage(int queueID, MachineStatusMessage* message);

// Clean up message queue
void cleanupMessageQueue(int msgQueueID);

#endif