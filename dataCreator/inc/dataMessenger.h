#ifndef DATAMESSENGER_H
#define DATAMESSENGER_H

#include "dataStructures.h"

void sendLoop(int msgid, char* statusMsg, int statusCode);
int sendMsg(MachineStatusMessage statusMessage, int msgid);

#endif // DATAMESSENGER_H