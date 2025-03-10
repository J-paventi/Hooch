#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include "dataStructures.h"

// Initialize shared memory
int initializeSharedMemory(MasterList** masterListPtr, int msgQueueID);

// Detach and remove shared memory
void cleanupSharedMemory(int shmID, MasterList* masterList);

#endif 