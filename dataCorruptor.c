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
// #include <> // shared memory





// MAKE SURE BEFORE SUBMISSION : no output is sent to the screen in the final version of the DX application


int dataCorrupter()
{
    key_t shmKey; // shared memory key
    int shmID; // shared memory ID

    // ********* TO DO ********* (variables to define and use 
 // pointer to master list (change to whatever the master list object is)
 // message queue ID 
 // 

    int retryCount = 0; 
    int retryLimit = 100;
    int sleepTimer = 10;

// Step 1: Attach to the shared memory created by the DR process
    // - Use ftok to generate a key
    
    if (generateKey(shmKey) == false)
    {
        printf( "\tERROR: Failed to generate a key with ftok\n"); 
        return -1;
    }

    // - Use shmget to get the shared memory ID
    while (getSharedMemoryID(shmID) == false)
    {
        // - If the shared memory is not created yet, sleep for 10 seconds and retry up to 100 times
        if (retryCount >= retryLimit) 
        {

            printf("\tERROR: Failed to attach to shared memory after %d retries\n", retryLimit); 
            return -1;

        }

        retryCount++;
        sleep(sleepTimer);
    }

    

    // Step 2: Ensure the DX application is running in the same directory as the DR application ***************** TO DO *****************





    // Step 3: Read the shared memory to gain knowledge of necessary information
    // - The shared memory contains a master list of all clients that the DR is in communication with ***************** TO DO *****************

    //int *sharedMemory = ;
   // if (sharedMemory == ) 
    {

        printf("\tERROR: Failed to read shared memory\n"); 
        return -1;

    }



    // Step 4: Main processing loop
    DX_MainLoop();



    // Step 5: Detach from the shared memory    ***************** TO DO *****************
    // - Use shmdt to detach from the shared memory


    return 0;


}





/*
* FUNCTION :           generateKey()  
* DESCRIPTION :        This function will generate a key for the shared memory using the ftok function
* PARAMETERS :         int shmkey - the key for the shared memory
* RETURNS :            bool - true if the key was generated successfully, false otherwise
*/
bool generateKey(shmkey)
{

    shmKey = ftok(".", 16535); 
    if (shmKey == -1)
    {
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

getSharedMemoryID(shmID)
{
    shmID = shmget(shmKey, sizeof(int), 0666);
    if (shmID == -1)
    {
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

int wheelOfDestruction()
{
    int action = rand() % 21; // random number between 0 and 20
    int processID = -1; // Process ID set to a number out of range to prevent accidental killing of a process

    switch(action)
    {
        case 0:
        // do nothing
        break;

    case 1:
        // kill DC-01 process
        processID = 1;
        killDC(processID);
        break;
    
    case 2:
        // kill DC-03 process
        processID = 3;
        killDC(processID);
        break;

    case 3:
        // kill DC-02 process
        processID = 2;
        killDC(processID);
        break;

    case 4:
        // kill DC-01 process
        processID = 1;
        killDC(processID);
        break;

    case 5:
        // kill DC-03 process
        processID = 3;
        killDC(processID);
        break;

    case 6:
        // kill DC-02 process
        processID = 2;
        killDC(processID);
        break;

    case 7:
        // kill DC-04 process
        processID = 4;
        killDC(processID);
        break;

    case 8:
        // do nothing
        break;

    case 9:
        // kill DC-05 process
        processID = 5;
        killDC(processID);
        break;

    case 10:
        // delete the message queue being used between DCs and DR   ********* TO DO *********
        break;

    case 11:
        // kill DC-01 process
        processID = 1;
        killDC(processID);
        break;

    case 12:
        // kill DC-06 process
        processID = 6;
        killDC(processID);
        break;

    case 13:
        // kill DC-02 process
        processID = 2;
        killDC(processID);
        break;

    case 14:
        // kill DC-07 process
        processID = 7;
        killDC(processID);
        break;

    case 15:
        // kill DC-03 process
        processID = 3;
        killDC(processID);
        break;

    case 16:
        // kill DC-08 process
        processID = 8;
        killDC(processID);
        break;

    case 17:
        // delete the message queue being used between DCs and DR ********* TO DO *********
        break;

    case 18:
        // kill DC-09 process
        processID = 9;
        killDC(processID);
        break;

    case 19:
        // do nothing
        break;

    case 20:
        // kill DC-10 process
        processID = 10;
        killDC(processID);
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
* PARAMETERS :         int processID - the ID of the process to kill
* RETURNS :            bool - true if the process was killed successfully, false otherwise         
*/

bool killDC(int processID)
{

    //if (kill(processID, SIGKILL) == -1)           ***************** TO DO ***************** (kill the process properly)
    {
        return false;
    }

    return true;
}





/*
* FUNCTION :           DX_MainLoop()
* DESCRIPTION :        This function will contain the main processing loop for the DX application
* PARAMETERS :         none
* RETURNS :            none
*/

void DX_MainLoop()
{

    // - Repeat the loop until the DR application is terminated
    while( true )                                                       // ***************** TO DO ***************** (loop until the app is terminated)
    {

    // - Sleep for a random amount of time (between 10 and 30 seconds)
    randomSleep();


    // - Check for the existence of the message queue between the DCs and the DR                                    ***************** TO DO *****************
    //   - If the message queue no longer exists, log the event, detach from shared memory, and exit


    // - Select an action from the Wheel of Destruction randomly
    int action = wheelOfDestruction();
    printf("Action taken: %d\n", action);

    }


}

/*
*   FUNCTION :          randomSleep()
*   DESCRIPTION :       This function will sleep for a random amount of time between 10 and 30 seconds
*   PARAMETERS :        none
*   RETURNS :           none
*/

void randomSleep()
{

    int sleepTime = (rand() % 21) + 10; // random number between 10 and 30 ********* TO DO ********* (double check this is working right / can be written differently)
    sleep(sleepTime);

}


/*
* FUNCTION :           logEvent()
* DESCRIPTION :        This function will log significant events
* PARAMETERS :         int event - the event to log
* RETURNS :            none
*/

void logEvent(int event)
{

    // - Log the event to a file                                    ***************** TO DO *****************
    // - Include a timestamp and a description of the event
    
}
