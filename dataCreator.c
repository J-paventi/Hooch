#include <time.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MIN_MSG_VAL      0
#define MAX_MSG_VAL      6
#define Q_CHECK_SLEEP   10
#define MIN_DELAY       10
#define MAX_DELAY       30
#define NO_QUEUE        -1
#define MAX_BUFFER      40

int getStatusMsg(void);
char* statusMsgToSend(int statusNum);
int getDelayInSeconds(void);
void delay(int numberOfSeconds);

void main(){
        // default the assumption of no queue to enter while-loop to check for queue
        int msgid = NO_QUEUE;
        // is this actually sending it, or just writing it? Need to verify this with Craig
        key_t key = ftok(".", 16535);           // I think this should connect to the message queue memory? I'm not too sure

        while(msgid == NO_QUEUE){
                msgid = msgget(key, IPC_EXCL | 0666);   // checks for the queue but does not make it
                delay(MIN_DELAY);
        }

        // this is always the very first status message sent to the DR, no exceptions!
        int status = 0;
        // put the code to send the message to the queue here and LOG IT

        while(status != 6){
                int seconds = getDelayInSeconds();
                delay(seconds);
                status = getStatusMsg();
                // somehow send the status message into the queue
                // log the status message
        }

        return;
}

int getStatusMsg(){
        int status = rand()%(MAX_MSG_VAL - MIN_MSG_VAL + 1) + MIN_MSG_VAL;

        return status;
}

char* statusMsgToSend(int statusNum){
        char statusRtrn[MAX_BUFFER] = NULL;

        switch(statusNum){
                case 0:
                        statusRtrn[MAX_BUFFER] = "Everything is OKAY.";
                        break;
                case 1:
                        statusRtrn[MAX_BUFFER] = "Hydraulic Press Failure.";
                        break;
                case 2:
                        statusRtrn[MAX_BUFFER] = "Safety Button Failure.";
                        break;
                case 3:
                        statusRtrn[MAX_BUFFER] = "No Raw Material in Processing.";
                        break;
                case 4:
                        statusRtrn[MAX_BUFFER] = "Operating Temperature Out of Range.";
                        break;
                case 5:
                        statusRtrn[MAX_BUFFER] = "Operator Error.";
                        break;
                case 6:
                        statusRtrn[MAX_BUFFER] = "Machine is Offline.";
                        break;
        }

        return statusRtrn;
}

int getDelayInSeconds(){
        int delayInSeconds = rand()%(MAX_DELAY - MIN_DELAY +1) + MIN_DELAY;

        return delayInSeconds;
}


void delay(int numberOfSeconds){
        int milliSeconds = 1000*numberOfSeconds;

        clock_t startTime = clock();

        while(clock() < startTime + milliSeconds);
}

