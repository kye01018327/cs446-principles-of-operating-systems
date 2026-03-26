#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <pthread.h>   // for pthreads
#include <time.h>  // for clock_gettime 

#define ARR_SIZE 2000000

typedef struct _thread_data_t {
    int localTid;
    const int *data;
    int numVals;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;

int main(int argc, char *argv[]) {
    // Check if two arguments have been provided
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        return -1;
    }
    int num_threads = atoi(argv[1]);
    
    // Dynamically allocate a fixed-sized array of 2,000,000 ints.
    int *data = (int *)malloc(sizeof(int) * ARR_SIZE);

    // Create and initialize to 0 a long long int totalSum variable
    long long int totalSum = 0;

    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    // Construct array of thread_data_t objects
    // number of objects should be the same as the number of threads
    thread_data_t td_arr[num_threads];
    for (int i = 0; i < num_threads; i++) {
        td_arr[i].localTid = i;
        td_arr[i].data = data;
        td_arr[i].numVals = ARR_SIZE;
        td_arr[i].lock = &lock;
        td_arr[i].totalSum = &totalSum;
    }

    pthread_t pthreads[num_threads];

    for (int i = 0; i < )

    
    free(data);
    return 0;
}
