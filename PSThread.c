#include "PSThread.h"
#include "CommonFuncs.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

extern char *fileName;
extern struct programData data;
extern struct threadargs threadParams[NUM_OF_THREADS];
extern int done[NUM_OF_THREADS];
extern struct LinkedList* readyQueue;

void runner(void *param)
{
    int threadId = ((struct threadargs *)param)->id;
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    printf("Thread %d begins\n", threadId);

    FILE *infile = NULL;
    if (fileName != NULL)
    {
        printf("Filename check for thread\n");
        char modifiedFileName[128];
        getFileName(modifiedFileName, threadId);
        printf("Thread with id: %d is opening arg file: %s\n", threadId, modifiedFileName);

        infile = fopen(modifiedFileName, "r");
    }

    //sleep(1);
    //waitForExecution[threadId] = 1;

    if (infile != NULL)
    {
        while (!feof(infile))
        {
            // Get Burst Duration
            int burstTime = getCPUBurstDuration(infile);

            // Send burst duration to queue
            // write code here by deleting this line -----!

            // wait for mutex conditional variable
            pthread_cond_wait(&threadParams[threadId].cond, &lock);

            // Get Sleep Duration
            int sleepTime = getSleepDuration(infile);
            sleep(0.001 * sleepTime); // Sleep in ms!!!!!
        }
    }
    else
    {
        printf("Else thread\n");
        int i = 0;
        while (i < data.duration) // this needs  to be improved if an infile is read!!!!!
        {
            printf("Duration1 %d\n", i);
            // Get Burst Duration
            int burstTime = getCPUBurstDuration(infile);
            printf("Duration2 %d\n", i);
            // Send burst duration to queue
            addNode(readyQueue, threadId, burstTime);

            // wait for mutex conditional variable
            pthread_cond_wait(&(threadParams[threadId].cond), &lock);
            printf("Duration3 %d\n", i);
            // Get Sleep Duration
            int sleepTime = getSleepDuration(infile);
            printf("Sleep time: %d\n", sleepTime);
            sleep(0.001 * sleepTime); // Sleep in ms!!!!!
            printf("Duration4 %d\n", i);
            i++;
        }
    }
    done[threadId] = 0;
    pthread_exit(0);
}

int getCPUBurstDuration(FILE *fp)
{
    int burstTime = 0;
    if (fp == NULL)
    {
        burstTime = getRandomNum(data.minCPU, data.maxCPU, 100);
        printf("BurstTime:%d\n", burstTime);
    }
    else
    {
        // READ burst time from file and assign it to burstTime
        fscanf(fp, "%*s %d", &burstTime);
    }
    return burstTime;
}

int getSleepDuration(FILE *fp)
{
    int sleepTime = 0;
    if (fp == NULL)
    {
        sleepTime = getRandomNum(data.minIO, data.maxIO, 100);
        printf("IO:%d\n", sleepTime);
    }
    else
    {
        // READ burst time from file and assign it to burstTime
        fscanf(fp, "%*s %d", &sleepTime);
    }
    return sleepTime;
}