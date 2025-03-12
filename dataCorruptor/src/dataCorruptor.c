/*
* FILE :               dataCorruptor.c   
* PROJECT :            System Programming A3 : Hoochamacallit System
* AUTHORS :            Kalina Cathcart, Josh Horsley, Jon Paventi, Daimon Quinn, Jiwoo (Tony) Yang
* FIRST VERSION :      2025 - 03 - 05
* DESCRIPTION :        This program holds the logic for the dataCorrupter program, which will test the execption paths of the other 2 programs in the assignment. 
*                      This program will gain knowledge of the resources and processes involved in the app suite and then randomly decide between a set of allowable corruptions.
*                      The shortform referece = DX
*                      Allowable Corruptions
*                            - kill a DC process (to test a DC application going offline in the application suite)
*                            - to delete the message queue being used between the set of DC applications and the DR application
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>

#include <signal.h>
#include <time.h>

#include "logger.h" // for logging functions
#include "shared_memory.h" // for shared memory functions
#include "message_queue.h" // for message queue functions
#include "data_structures.h"

// Function prototypes 
bool generateKey(key_t *shmKey);
bool getSharedMemoryID(key_t shmKey, int *shmID);
void DX_MainLoop(MasterList *masterList, int shmID);
void randomSleep();
int wheelOfDestruction(MasterList *masterList);
bool killDC(MasterList *masterList, int index);
bool deleteMessageQueue();


/*
* FUNCTION :           main()
* DESCRIPTION :        This function will contain the main logic for the dataCorruptor program. 
*                      It will attach to the shared memory created by the DR process. 
*                      It will then read the shared memory to gain knowledge of necessary information,
* PARAMETERS :         none
* RETURNS :            int - 0 if the program executed successfully, -1 otherwise
*/

int main()
{
    key_t shmKey; // shared memory key
    int shmID; // shared memory ID
    MasterList *masterList; // master list of all clients that the DR is in communication with


    int retryCount = 0; 
    int retryLimit = 100;
    int sleepTimer = 10;


    logMessage("\n\n---DX Data Corruption Process Started---");

    // test purpose ============= ERASE LATER =============
    printf("DX Data Corruption Process Started\n");


    // Step 1: Attach to the shared memory created by the DR process
    // - Use ftok to generate a key
    
    if (generateKey(&shmKey) == false)
    {
        logMessage("ERROR: Failed to generate a key with ftok");

        // test purpose ============= ERASE LATER =============
        printf("ERROR: Failed to generate a key with ftok\n");

        return -1;
    }

    // - Use shmget to get the shared memory ID
    while (getSharedMemoryID(shmKey, &shmID) == false)
    {
        // - If the shared memory is not created yet, sleep for 10 seconds and retry up to 100 times
        if (retryCount >= retryLimit) 
        {

            logMessage("ERROR: Failed to attach to shared memory after 100 retries");

            // test purpose ============= ERASE LATER =============
            printf("ERROR: Failed to attach to shared memory after 100 retries\n");

            return -1;

        }

        retryCount++;
        sleep(sleepTimer);

        // test purpose ============= ERASE LATER =============
        printf("Failed to get shared memory ID, retrying... %d\n", retryCount);
    }




    // Step 3: Read the shared memory to gain knowledge of necessary information
    // - The shared memory contains a master list of all clients that the DR is in communication with ***************** TO DO *****************

    masterList = (MasterList *)shmat(shmID, NULL, 0);
    if (masterList == (void*)-1)
    {
        logMessage("ERROR: Failed to read shared memory");

        // test purpose ============= ERASE LATER =============
        printf("ERROR: Failed to read shared memory\n");

        return -1;
    }



    // Step 4: Main processing loop
    DX_MainLoop(masterList, shmID);

    // test purpose ============= ERASE LATER =============
    printf("DX Main Loop Ended\n");

    // Step 5: Detach from the shared memory    ***************** TO DO *****************
    // - Use shmdt to detach from the shared memory
    if (shmdt(masterList) == -1)
    {
        logMessage("ERROR: Failed to detach from shared memory");

        // test purpose ============= ERASE LATER =============
        printf("ERROR: Failed to detach from shared memory\n");

        return -1;
    }

    logMessage("---DX Data Corruption Process Ended---\n\n");

    // test purpose ============= ERASE LATER =============
    printf("DX Data Corruption Process Ended\n");

    return 0;


}



/*
* FUNCTION :           DX_MainLoop()
* DESCRIPTION :        This function will contain the main processing loop for the DX application
* PARAMETERS :         none
* RETURNS :            none
*/

void DX_MainLoop(MasterList *masterList, int shmID)
{

    // - Repeat the loop until the DR application is terminated
    while( true )                                                       // ***************** TO DO ***************** (loop until the app is terminated)
    {


        // Check for termination condition, if true, then DR is closed and DX should also end
        if (masterList->terminateDR)
        {
            logMessage("DR application terminated. Exiting DX_MainLoop.");
            break;
        }

        // - Sleep for a random amount of time (between 10 and 30 seconds)
        randomSleep();


        // - Check for the existence of the message queue between the DCs and the DR                                    ***************** TO DO *****************
            //   - If the message queue no longer exists, log the event, detach from shared memory, and exit

            int msgQueueId = msgget(masterList->msgQueueID, 0666);
            if (msgQueueId == -1)
            {
                logMessage("The DX app detected that msgQ is gone. Assuming DR/DCs done");
                return;
            }


        // - Select an action from the Wheel of Destruction randomly
        int action = wheelOfDestruction(masterList);
        char logMsg[100];
        sprintf(logMsg, "Wheel of Destruction Action: %d", action);
        logMessage(logMsg);

    }


}





/*
* FUNCTION :           generateKey()  
* DESCRIPTION :        This function will generate a key for the shared memory using the ftok function
* PARAMETERS :         key_t shmkey - the key for the shared memory
* RETURNS :            bool - true if the key was generated successfully, false otherwise
*/
bool generateKey(key_t *shmKey)
{

    *shmKey = ftok(".", 16535); 
    if (*shmKey == -1)
    {
        logMessage("ERROR: Failed to generate key with ftok");
        return false;
    }

    return true;

}




/*
* FUNCTION :           getSharedMemoryID()
* DESCRIPTION :        This function will get the shared memory ID using the shmget function
* PARAMETERS :         int shmID - the ID for the shared memory
* RETURNS :             bool - true if the ID was generated successfully, false otherwise
*/

bool getSharedMemoryID(key_t shmKey, int *shmID)
{
    *shmID = shmget(shmKey, sizeof(MasterList), 0660);
    if (*shmID == -1)
    {
        logMessage("ERROR: Failed to get shared memory ID with shmget");
        return false;
    }
    return true;
}




/*
* FUNCTION :          wheelOfDestruction()
* DESCRIPTION :       This function will randomly select an action from the Wheel of Destruction
* PARAMETERS :        none
* RETURNS :           int - the selected action
*/

int wheelOfDestruction(MasterList *masterList)
{
    int action = rand() % 21; // random number between 0 and 20
   

    switch(action)
    {
        case 0:
        // do nothing
        break;

    case 1:
        // kill DC-01 process
        killDC(masterList, 0);
        break;
    
    case 2:
        // kill DC-03 process
        killDC(masterList, 2);
        break;

    case 3:
        // kill DC-02 process
        killDC(masterList, 1);
        break;

    case 4:
        // kill DC-01 process
        killDC(masterList, 0);
        break;

    case 5:
        // kill DC-03 process
        killDC(masterList, 2);
        break;

    case 6:
        // kill DC-02 process
        killDC(masterList, 1);
        break;

    case 7:
        // kill DC-04 process
        killDC(masterList, 3);
        break;

    case 8:
        // do nothing
        break;

    case 9:
        // kill DC-05 process
        killDC(masterList, 4);
        break;

    case 10:
        // delete the message queue being used between DCs and DR  
        deleteMessageQueue();
        break;

    case 11:
        // kill DC-01 process
        killDC(masterList, 0);
        break;

    case 12:
        // kill DC-06 process
        killDC(masterList, 5);
        break;

    case 13:
        // kill DC-02 process
        killDC(masterList, 1);
        break;

    case 14:
        // kill DC-07 process
        killDC(masterList, 6);
        break;

    case 15:
        // kill DC-03 process
        killDC(masterList, 2);
        break;

    case 16:
        // kill DC-08 process
        killDC(masterList, 7);
        break;

    case 17:
        // delete the message queue being used between DCs and DR 
        deleteMessageQueue();
        break;

    case 18:
        // kill DC-09 process
        killDC(masterList, 8);
        break;

    case 19:
        // do nothing
        break;

    case 20:
        // kill DC-10 process
        killDC(masterList, 9);
        break;

    default:
        // do nothing
        break;
    }



    return action;
}





/*
* FUNCTION :           killDC()
* DESCRIPTION :        This function will kill a DC process
* PARAMETERS :         MasterList *masterList - pointer to the master list in shared memory
*                      int index - the index of the DC process in the master list
* RETURNS :            bool - true if the process was killed successfully, false otherwise         
*/

bool killDC(MasterList *masterList, int index)
{
    char logMsg[100];

    // Check if the index is valid
    if (index < 0 || index >= masterList->numberOfDCs)
    {
        sprintf(logMsg, "ERROR: Invalid index %d", index);
        logMessage(logMsg);
        return false;
    }

    // Get the process ID from the master list
    pid_t processID = masterList->dc[index].dcProcessID;

    // Check if the process ID is valid
    if (processID <= 0)
    {
        sprintf(logMsg, "ERROR: Invalid process ID %d for index %d", processID, index);
        logMessage(logMsg);
        return false;
    }

    // Attempt to kill the process
    if (kill(processID, SIGKILL) == -1)
    {
        sprintf(logMsg, "ERROR: Failed to kill process %d", processID);
        logMessage(logMsg);
        return false;
    }

    // Log the successful kill
    sprintf(logMsg, "Successfully killed process %d", processID);
    logMessage(logMsg);

    return true;
}





/*
*   FUNCTION :          randomSleep()
*   DESCRIPTION :       This function will sleep for a random amount of time between 10 and 30 seconds
*   PARAMETERS :        none
*   RETURNS :           none
*/

void randomSleep()
{

    int sleepTime = 10 + (rand() % 21); 
    sleep(sleepTime);

}




/*
* FUNCTION :          deleteMessageQueue()
* DESCRIPTION :       This function will delete the message queue being used between the DCs and the DR application
* PARAMETERS :        none          
* RETURNS :           bool - true if the message queue was deleted successfully, false otherwise              
*/

bool deleteMessageQueue()
{

    key_t msgKey;
    int msgQueueID;

    // Generate the key for the message queue
    msgKey = ftok(".", MSG_KEY);
    if (msgKey == -1) 
    {
        perror("ftok");
        return false;
    }

    // Get the message queue ID
    msgQueueID = msgget(msgKey, 0666);
    if (msgQueueID == -1) 
    {
        perror("msgget");
        return false;
    }

    // Delete the message queue
    if (msgctl(msgQueueID, IPC_RMID, NULL) == -1) 
    {
        perror("msgctl");
        return false;
    }

    return true;


}
