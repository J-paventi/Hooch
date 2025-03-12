#include <stdio.h> 
#include <string.h> 
#include <time.h> 
#include <stdlib.h> 
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <unistd.h>

#define MIN_MSG_VAL      0
#define MAX_MSG_VAL      6
#define Q_CHECK_SLEEP   10
#define MIN_DELAY       10
#define MAX_DELAY       30
#define NO_QUEUE        -1
#define MAX_BUFFER      40

typedef struct {
    long msgType;
    pid_t machineID;
    int statusCode;
    char statusMessage[256];
} MachineStatusMessage;


int getRandomStatusMsg(void);
char* statusMsgToSend(int statusNum);
int getDelayInSeconds(void);
void delay(int numberOfSeconds);
void logStatus(char* logMsg, int statusCode);



int main(void){ 
        FILE* logFP;
        int statusCode = 0;
        char statusMsg[MAX_BUFFER];

        // default the assumption of no queue to enter while-loop to check for queue 
        int msgid = NO_QUEUE; 
        // is this actually sending it, or just writing it? Need to verify this with Craig 
        key_t key = ftok(".", 16535);           // I think this should connect to the message queue memory? I'm not too sure 
        
        while(msgid == NO_QUEUE){ 
                msgid = msgget(key, 0666);   // checks for the queue but does not make it 
                delay(MIN_DELAY); 
        } 
        
        logStatus(strcpy(statusMsg, statusMsgToSend(statusCode)), statusCode);

        while(statusCode != 6){
                int seconds = getDelayInSeconds();
                delay(seconds);
                statusCode = getRandomStatusMsg();
                logStatus(strcpy(statusMsg, statusMsgToSend(statusCode)), statusCode); 

                MachineStatusMessage message;
                message.msgType = 1;  // Example type
                message.machineID = getpid();  // Use the process ID as the machine ID
                message.statusCode = statusCode;
                strcpy(message.statusMessage, statusMsg);

                if (msgsnd(msgid, &message, sizeof(MachineStatusMessage) - sizeof(long), 0) == -1) {
                        perror("Failed to send message");
                } else {
                        printf("Message sent: %s\n", message.statusMessage);
                }
        }
                // send status message somehow
        
 
        return;  
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

        fprintf(logFP, "[%d-%d-%d]: DC[] - MSG SENT - Status [%d] (%s)", year, month, day, statusNum, logMsg);
        fclose(logFP);
}

int getRandomStatusMsg(){
        int status = rand()%(MAX_MSG_VAL - MIN_MSG_VAL + 1) + MIN_MSG_VAL;

        return status;
}

char* statusMsgToSend(int statusNum) {
    static char statusRtrn[MAX_BUFFER];  // Static so it persists after the function returns

    switch (statusNum) {
        case 0: strcpy(statusRtrn, "Everything is OKAY."); break;
        case 1: strcpy(statusRtrn, "Hydraulic Press Failure."); break;
        case 2: strcpy(statusRtrn, "Safety Button Failure."); break;
        case 3: strcpy(statusRtrn, "No Raw Material in Processing."); break;
        case 4: strcpy(statusRtrn, "Operating Temperature Out of Range."); break;
        case 5: strcpy(statusRtrn, "Operator Error."); break;
        case 6: strcpy(statusRtrn, "Machine is Offline."); break;
        default: strcpy(statusRtrn, "Unknown Status."); break;
    }

    return statusRtrn;
}

int getDelayInSeconds(){
        int delayInSeconds = rand()%(MAX_DELAY - MIN_DELAY +1) + MIN_DELAY;

        return delayInSeconds;
}



void delay(int numberOfSeconds) {
    sleep(numberOfSeconds);
}
