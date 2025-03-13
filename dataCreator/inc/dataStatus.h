#ifndef DATASTATUS_H
#define DATASTATUS_H

int getRandomStatusMsg(void);
char* statusMsgToSend(int statusNum);
int getDelayInSeconds(void);
void logStatus(char* logMsg, int statusCode);

#endif // DATASTATUS_H