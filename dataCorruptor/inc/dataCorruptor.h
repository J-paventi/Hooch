#ifndef DATA_CORRUPTOR_H
#define DATA_CORRUPTOR_H

#include "logger.h"
#include "sharedMemory.h"
#include "messageQueue.h"

bool generateKey(key_t *shmKey);

bool getSharedMemoryID(key_t shmKey, int *shmID);

void DX_MainLoop(MasterList *masterList, int shmID);

void randomSleep();

int wheelOfDestruction(MasterList *masterList);

bool killDC(MasterList *masterList, int index);

bool deleteMessageQueue();

#endif