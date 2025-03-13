/*
* FILE : dataStatus.c
* PROJECT : Hoochamacallit System
* PROGRAMMERS : Josh Horsley, Kalina Cathcart, John Paventi, Daimon Quin, Tony Yang
* FIRST VERSION : 2025-03-09
* UPDATED : 2025-03-12
* DESCRIPTION : 
* This file contains functions for generating and logging status messages for the dataCreator program.
* The getRandomStatusMsg function generates a random status code.
* The statusMsgToSend function converts a status code to a corresponding status message.
* The getDelayInSeconds function generates a random delay time.
* The logStatus function logs the status message to a log file.
* 
* FUNCTIONS:
* - getRandomStatusMsg : Generates a random status code.
* - statusMsgToSend : Converts a status code to a corresponding status message.
* - getDelayInSeconds : Generates a random delay time.
* - logStatus : Logs the status message to a log file.
*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "dataStatus.h"
#include "dataStructures.h"

#define MIN_MSG_VAL      0
#define MAX_MSG_VAL      6
#define MIN_DELAY       10
#define MAX_DELAY       30
#define MAX_BUFFER      40

/*
* FUNCTION : getRandomStatusMsg
* DESCRIPTION : 
* Generates a random status code.
* PARAMETERS :
* - None
* RETURNS : 
* - int : A random status code between MIN_MSG_VAL and MAX_MSG_VAL.
*/
int getRandomStatusMsg(){
    int status = rand()%(MAX_MSG_VAL - MIN_MSG_VAL + 1) + MIN_MSG_VAL;
    return status;
}

/*
* FUNCTION : statusMsgToSend
* DESCRIPTION : 
* Converts a status code to a corresponding status message.
* PARAMETERS :
* - int statusNum : The status code.
* RETURNS : 
* - char* : The corresponding status message.
*/
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

/*
* FUNCTION : getDelayInSeconds
* DESCRIPTION : 
* Generates a random delay time.
* PARAMETERS :
* - None
* RETURNS : 
* - int : A random delay time between MIN_DELAY and MAX_DELAY seconds.
*/
int getDelayInSeconds(){
    int delayInSeconds = rand()%(MAX_DELAY - MIN_DELAY +1) + MIN_DELAY;
    return delayInSeconds;
}

/*
* FUNCTION : logStatus
* DESCRIPTION : 
* Logs the status message to a log file.
* PARAMETERS :
* - char* logMsg : The status message to log.
* - int statusNum : The status code.
* RETURNS : 
* - void
*/
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