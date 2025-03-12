/*
* FILE : logger.c
* PROJECT : Hoochamacallit System
* PROGRAMMERS : Josh Horsley, 
* FIRST VERSION : 2025-03-09
* UPDATED : 2025-03-09
* DESCRIPTION :
* This file contains the implementation of the logMessage function. The function is responsible for logging
* messages to a log file, with a timestamp prepended to each entry. The log file is opened in append mode,
* ensuring that new log entries are added to the end of the file. If the log file is successfully opened,
* the message is written, and the file is closed afterward.
* 
* FUNCTIONS:
* - logMessage : Opens the log file, adds a timestamp to the provided message, and writes it to the log file.
*
*/

#include "logger.h"
#include "dataStructures.h"
#include <stdio.h>
#include <time.h>

void logStatus(char* logMsg, int statusNum, const char* logFilePath) {
    FILE* logFP = fopen(logFilePath, "a");
    if (logFP == NULL) {
        perror("Failed to open log file");
        return;
    }

    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(logFP, "[%s] : Status [%d] - %s\n", timestamp, statusNum, logMsg);
    fclose(logFP);
}