#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>  

int readFile(char[], int[]);
void *arraySum(void *);

typedef struct _thread_data_t {
    const int* data;
    int startInd;
    int endInd;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;

int main(int argc, char* argv[]) {
if (argc != 3) {
        printf("Three arguments not provided, exiting program ");
        return -1;
    }
    long long maxValues = 1000000000;
    int *data = malloc(sizeof(int) * maxValues);
    int numValues = readFile(argv[1], data);
    int numThreads = atoi(argv[2]);
    
    if (numThreads >= numValues) {
        printf("Too many threads requested\n");
        return -1;
    }
    
    long long int totalSum = 0;
    struct timeval startTime;
    gettimeofday(&startTime, NULL);
    
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    
    thread_data_t threadData[numThreads];
    
    int sliceSize = numValues / numThreads;
    int remainder = numValues % numThreads;
    int startIndex = 0;

    for (int i = 0; i < numThreads; i++) {
        threadData[i].data = data;
        threadData[i].lock = &lock;
        threadData[i].totalSum = &totalSum;

        threadData[i].startInd = startIndex;
        threadData[i].endInd = startIndex + sliceSize;
        if (i == numThreads - 1) {
            threadData[i].endInd += remainder;
        }
        startIndex = threadData[i].endInd;
    }

    
    pthread_t threads[numThreads];
    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, arraySum, &threadData[i]);
    }
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
    struct timeval endTime;
    gettimeofday(&endTime, NULL); 
    double executionTime = (endTime.tv_sec - startTime.tv_sec) * 1000.0 
        + (endTime.tv_usec - startTime.tv_usec) / 1000.0;
    printf("Execution time (milliseconds): %f\n", executionTime);
    printf("Final Sum: %lld\n", totalSum);

    pthread_mutex_destroy(&lock);
    free(data);
    return 0;
}

int readFile(char fileName[], int intArr[]) {
    FILE *fp;
    fp = fopen(fileName, "r");

    if (fp == NULL) {
        printf("File not found...\n");
        return -1;
    }
    int i = 0;
    while (fscanf(fp, "%d", &intArr[i]) == 1) {
        i++;
    }
    fclose(fp);
    return i;
}

void *arraySum(void *input) {
    thread_data_t *threadData = (thread_data_t *) input;
    long long int threadSum = 0;
    for (int i = threadData->startInd; i < threadData->endInd; i++) {
        threadSum += threadData->data[i];
    }

    pthread_mutex_lock(threadData->lock);
    *(threadData->totalSum) += threadSum;
    pthread_mutex_unlock(threadData->lock);

    pthread_exit(NULL);
}
