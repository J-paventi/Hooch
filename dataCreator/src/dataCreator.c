/*
* FILE : dataCreator.c
* PROJECT : Hoochamacallit System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-09
* UPDATED : 2025-03-12
* DESCRIPTION : The dataCreator program simulates a piece of machinery that periodically reports 
*               its status to a server. It generates random status messages, such as "Everything is 
*               OKAY" or "Hydraulic Press Failure," and sends these messages to a message queue. 
*               The program ensures that the message queue exists before starting the reporting loop. 
*               It uses functions from dataMessenger and dataStatus to handle message sending and status 
*               generation, respectively. The main loop continues to send status updates until a specific 
*               termination condition is met, mimicking the behavior of a real machine monitoring system.
* 
* FUNCTIONS:
* - main : The main loop that checks for message queue then sends randomized status messages.
*/
#include <stdio.h> 
#include <string.h> 
#include <time.h> 
#include <stdlib.h> 
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <unistd.h>
#include "dataStructures.h"
#include "dataMessenger.h"
#include "dataStatus.h"

#define MIN_MSG_VAL      0
#define MAX_MSG_VAL      6
#define Q_CHECK_SLEEP   10
#define MIN_DELAY       10
#define MAX_DELAY       30
#define NO_QUEUE        -1
#define MAX_BUFFER      40


int main(void){ 
    int statusCode = 0;
    char statusMsg[MAX_BUFFER];

    FILE *file = fopen("/tmp/keyfile", "w");
    if (file == NULL) {
        perror("Failed to create file for ftok");
        exit(EXIT_FAILURE);
    }
    fclose(file);

    int msgid = NO_QUEUE; 
    key_t key = ftok("/tmp/keyfile", MSG_KEY); 
    if (key == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
     
    while(msgid == NO_QUEUE){ 
        msgid = msgget(key, IPC_EXCL | 0666);
        if (msgid == NO_QUEUE) {
            perror("Waiting for message queue to be created...\n");
        }
        sleep(MIN_DELAY); 
    } 

    sendLoop(msgid, statusMsg, statusCode);

    return 0;  
} 