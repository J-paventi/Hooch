#include "message_queue.h"
#include "logger.h"
#include <sys/msg.h>
#include <sys/ipc.h>

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

int receiveMessage(int queueID, MachineStatusMessage* message) {
    if (msgrcv(queueID, message, sizeof(*message) - sizeof(long), 0, 0) == -1) {
        logMessage("msgrcv failed");
        return -1;
    }
    return 0;
}

void cleanupMessageQueue(int msgQueueID) {
    if (msgctl(msgQueueID, IPC_RMID, NULL) == -1) {
        logMessage("Failed to remove message queue");
    }
}