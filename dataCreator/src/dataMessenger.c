/*
* FILE : dataMessenger.c
* PROJECT : Hoochamacallit System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-09
* UPDATED : 2025-03-12
* DESCRIPTION : 
* This file contains functions for sending status messages from the dataCreator program to a message queue.
* The sendLoop function continuously generates and sends status messages until a termination condition is met.
* The sendMsg function handles the actual sending of messages to the message queue.
* 
* FUNCTIONS:
* - sendLoop : The main loop that generates and sends randomized status messages.
* - sendMsg : Sends a status message to the message queue.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "dataMessenger.h"
#include "dataStatus.h"
#include "dataStructures.h"

/*
* FUNCTION : sendLoop
* DESCRIPTION : 
* The main loop that generates and sends randomized status messages to the message queue.
* PARAMETERS :
* - int msgid : The message queue ID.
* - char* statusMsg : The buffer to hold the status message.
* - int statusCode : The initial status code.
* RETURNS : 
* - void
*/
void sendLoop(int msgid, char* statusMsg, int statusCode)
{
    while(statusCode != 6){
        int seconds = getDelayInSeconds();
        sleep(seconds);
        statusCode = getRandomStatusMsg();
        logStatus(strcpy(statusMsg, statusMsgToSend(statusCode)), statusCode); 

        MachineStatusMessage statusMessage;
        statusMessage.msgType = 1;
        statusMessage.machineID = getpid();
        statusMessage.statusCode = statusCode;
        strncpy(statusMessage.statusMessage, statusMsg, sizeof(statusMessage.statusMessage) - 1);
        statusMessage.statusMessage[sizeof(statusMessage.statusMessage) - 1] = '\0';

        if (sendMsg(statusMessage, msgid) == -1) {
            printf("Error: Failed to send message\n");
            exit(EXIT_FAILURE);
        } else {
            printf("Message sent: %d\n", statusMessage.statusCode);
        }
    }
}

/*
* FUNCTION : sendMsg
* DESCRIPTION : 
* Sends a status message to the message queue.
* PARAMETERS :
* - MachineStatusMessage statusMessage : The status message to be sent.
* - int msgid : The message queue ID.
* RETURNS : 
* - int : 0 on success, -1 on failure.
*/
int sendMsg(MachineStatusMessage statusMessage, int msgid)
{
    if(msgsnd(msgid, &statusMessage, sizeof(statusMessage) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        return -1;
    }
    return 0;
}