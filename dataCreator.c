#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MIN_MSG_VAL	  0;
#define MAX_MSG_VAL	  6;
#define Q_CHECK_SLEEP	 10;
#define MIN_DELAY	 10;
#define MAX_DELAY	 30;
#define NO_QUEUE	 -1;

void dataCreator(){
	// default the assumption of no queue to enter while-loop to check for queue
	int msgid = NO_QUEUE;
	key_t key = ftok(".", 16535);		// I think this should connect to the message queue memory? I'm not too sure

	while(msgid == NO_QUEUE){
		msgid = msgget(key, IPC_EXCL | 0666);	// checks for the queue but does not make it
		delay(Q_CHECK_SLEEP);
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

int getDelayInSeconds{
	int delayInSeconds = rand()%(MAX_DELAY - MIN_DELAY +1) + MIN_DELAY;

	return delayInSeconds;	
}


void delay(int numberOfSeconds){
	int milliSeconds = 1000*numberOfSeconds;

	clock_t startTime = clock();

	while(clock() < startTime + milliSeconds);
}
