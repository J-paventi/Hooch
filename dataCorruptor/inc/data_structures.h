#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>

#define MAX_DC_ROLES 10
#define SHM_KEY 16535
#define MSG_KEY 1234 //temporary key number
#define INACTIVE_THRESHOLD 35 //seconds
#define LOG_FILE "tmp/dataMonitor.log"

typedef struct {
    pid_t dcProcessID;
    time_t lastTimeHeardFrom;
} DCInfo;

typedef struct {
    int msgQueueID;
    int numberOfDCs;
    bool terminateDR;
    DCInfo dc[MAX_DC_ROLES];
} MasterList;

typedef struct {
    long msgType;
    pid_t machineID;
    int statusCode;
    char statusMessage[256];
} MachineStatusMessage;


#endif // DATA_STRUCTURES_H
