#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MIN_MSG_VAL	  0;
#define MAX_MSG_VAL	  6;
#define MIN_DELAY	 10;
#define MAX_DELAY	 30;
#define NO_QUEUE	 -1;

void dataCreator(){
	// Code to check for message queue goes here
	// check queue
	// loop start if no queue
		// wait 10s
		// if queue exit loop
	// loop end
	int msgid = NO_QUEUE;
	key_t key = ftok(".", 16535);

	while(msgid == NO_QUEUE){
		msgid = msgget(key, IPC_EXCL | 0666);
		int sleepDelay = getDelayInSeconds();
		delay(sleepDelay);
	}

	// this is always the very first status message sent to the DR, no exceptions!
	int status = 0;

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
