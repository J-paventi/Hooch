#ifndef DC_MANAGER_H
#define DC_MANAGER_H

#include "data_structures.h"

// Add or update DC in master list
void updateDCStatus(MasterList* masterList, pid_t machineID, int statusCode, const char* statusMessage);

// Remove machine from list
void removeMachineFromList(MasterList* masterList, int index, int shmID, int msgQueueID);

// Check for inactive machines
void checkInactiveMachines(MasterList* masterList, int shmID, int msgQueueID);

// Complete system cleanup
void cleanup(int shmID, int msgQueueID, MasterList* masterList);

#endif 