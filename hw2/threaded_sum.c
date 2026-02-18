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

typedef struct _thread_data_t {
    const int* data;
    int startInd;
    int endInd;
    pthread_mutex_t *lock;
    long long int *totalSum;
} threaded_data_t;

int main(int argc, char* argv[]) {
    printf("Program start\n");
    for (int i = 0; i < argc; i++) {
        printf("%d: %s\n", i, argv[i]);
    }

    if (argc != 3) {
        printf("Three arguments not provided, exiting program ");
        return -1;
    }
    int intArr[1000];
    printf("%d", readFile(argv[1], intArr));

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
