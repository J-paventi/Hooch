#include <stdio.h> 
#include <string.h> 
#include <time.h> 
#include <stdlib.h> 
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <unistd.h>
#include "dataStructures.h"

#define MIN_MSG_VAL      0
#define MAX_MSG_VAL      6
#define Q_CHECK_SLEEP   10
#define MIN_DELAY       10
#define MAX_DELAY       30
#define NO_QUEUE        -1
#define MAX_BUFFER      40

int getRandomStatusMsg(void);
char* statusMsgToSend(int statusNum);
int getDelayInSeconds(void);
void logStatus(char* logMsg, int statusCode);
int sendMsg(MachineStatusMessage statusMessage, int msgid);

int main(void){ 
    int statusCode = 0;
    char statusMsg[MAX_BUFFER];

    // Ensure the file for ftok exists
    FILE *file = fopen("/tmp/keyfile", "w");
    if (file == NULL) {
        perror("Failed to create file for ftok");
        exit(EXIT_FAILURE);
    }
    fclose(file);

    // default the assumption of no queue to enter while-loop to check for queue 
    int msgid = NO_QUEUE; 
    key_t key = ftok("/tmp/keyfile", MSG_KEY); 
    if (key == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
     
    while(msgid == NO_QUEUE){ 
        msgid = msgget(key, IPC_EXCL | 0666); // checks for the queue but does not make it 
        if (msgid == NO_QUEUE) {
            printf("Waiting for message queue to be created...\n");
        }
        sleep(MIN_DELAY); 
    } 
    
    printf("Message queue found. Starting to send messages...\n");
    logStatus(strcpy(statusMsg, statusMsgToSend(statusCode)), statusCode);

    while(statusCode != 6){
        int seconds = getDelayInSeconds();
        sleep(seconds);
        statusCode = getRandomStatusMsg();
        logStatus(strcpy(statusMsg, statusMsgToSend(statusCode)), statusCode); 

        // Prepare the message
        MachineStatusMessage statusMessage;
        statusMessage.machineID = getpid();
        statusMessage.statusCode = statusCode;
        strncpy(statusMessage.statusMessage, statusMsg, sizeof(statusMessage.statusMessage) - 1);
        statusMessage.statusMessage[sizeof(statusMessage.statusMessage) - 1] = '\0'; // Ensure null-termination

        // Send the message
        if (sendMsg(statusMessage, msgid) == -1) {
            printf("Error: Failed to send message\n");
            exit(EXIT_FAILURE);
        } else {
            printf("Message sent: %d\n", statusMessage.statusCode);
        }
    }

    return 0;  
} 

void logStatus(char* logMsg, int statusNum){
    FILE* logFP;
    int day;
    int month;
    int year;
    struct tm *info;
    time_t currentTime; 

    currentTime = time(NULL);
    info = localtime(&currentTime);
    day = info->tm_mday;
    month = info->tm_mon + 1;
    year = info->tm_year + 1900; 
    logFP = fopen("/tmp/dataCreator.log", "a+");
    if (logFP == NULL) {
        perror("Failed to open log file");
        return;
    }
    fprintf(logFP, "[%d-%d-%d]: DC[] - MSG SENT - Status [%d] (%s)\n", year, month, day, statusNum, logMsg);
    fclose(logFP);
}

int getRandomStatusMsg(){
    int status = rand()%(MAX_MSG_VAL - MIN_MSG_VAL + 1) + MIN_MSG_VAL;
    return status;
}

char* statusMsgToSend(int statusNum){
    static char statusRtrn[MAX_BUFFER];

    switch(statusNum)
    {
        case 0:
            strncpy(statusRtrn, "Everything is OKAY.", MAX_BUFFER);
            break;
        case 1:
            strncpy(statusRtrn, "Hydraulic Press Failure.", MAX_BUFFER);
            break;
        case 2:
            strncpy(statusRtrn, "Safety Button Failure.", MAX_BUFFER);
            break;
        case 3:
            strncpy(statusRtrn, "No Raw Material in Processing.", MAX_BUFFER);
            break;
        case 4:
            strncpy(statusRtrn, "Operating Temperature Out of Range.", MAX_BUFFER);
            break;
        case 5:
            strncpy(statusRtrn, "Operator Error.", MAX_BUFFER);
            break;
        case 6:
            strncpy(statusRtrn, "Machine is Offline.", MAX_BUFFER);
            break;
        default:
            strncpy(statusRtrn, "Unknown Status.", MAX_BUFFER);
            break;
    }

    statusRtrn[MAX_BUFFER - 1] = '\0';

    return statusRtrn;
}

int getDelayInSeconds(){
    int delayInSeconds = rand()%(MAX_DELAY - MIN_DELAY +1) + MIN_DELAY;
    return delayInSeconds;
}
int sendMsg(MachineStatusMessage statusMessage, int msgid)
{
    if(msgsnd(msgid, &statusMessage, sizeof(statusMessage) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
        return -1;
    }
    return 0;
}
