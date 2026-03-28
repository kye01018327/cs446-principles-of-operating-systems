#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <pthread.h>   // for pthreads
#include <time.h>  // for clock_gettime 

#define ANSI_COLOR_GRAY    "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"

#define ANSI_COLOR_RESET   "\x1b[0m"

#define TERM_CLEAR() printf("\033[H\033[J")
#define TERM_GOTOXY(x,y) printf("\033[%d;%dH", (y), (x))

#define ARR_SIZE 2000000

typedef struct _thread_data_t {
    int localTid;
    const int *data;
    int numVals;
    pthread_mutex_t *lock;
    long long int *totalSum;
} thread_data_t;


void *testFunc(void *input) {
    thread_data_t *td = (thread_data_t *)input;
    printf("Thread %d executed\n", td->localTid);
    return NULL;
}

void print_progress(pid_t localTid, size_t value) {
        pid_t tid = syscall(__NR_gettid);

        TERM_GOTOXY(0,localTid+1);

	char prefix[256];
        size_t bound = 100;
        sprintf(prefix, "%d: %ld (ns) \t[", tid, value);
	const char suffix[] = "]";
	const size_t prefix_length = strlen(prefix);
	const size_t suffix_length = sizeof(suffix) - 1;
	char *buffer = calloc(bound + prefix_length + suffix_length + 1, 1);
	size_t i = 0;

	strcpy(buffer, prefix);
	for (; i < bound; ++i)
	{
	    buffer[prefix_length + i] = i < value/300000 ? '#' : ' ';
	}
	strcpy(&buffer[prefix_length + i], suffix);
        
        if (!(localTid % 7)) 
            printf(ANSI_COLOR_WHITE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 6)) 
            printf(ANSI_COLOR_BLUE "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 5)) 
            printf(ANSI_COLOR_RED "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 4)) 
            printf(ANSI_COLOR_GREEN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 3)) 
            printf(ANSI_COLOR_CYAN "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 2)) 
            printf(ANSI_COLOR_YELLOW "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else if (!(localTid % 1)) 
            printf(ANSI_COLOR_MAGENTA "\b%c[2K\r%s\n" ANSI_COLOR_RESET, 27, buffer);  
        else
            printf("\b%c[2K\r%s\n", 27, buffer);

	fflush(stdout);
	free(buffer);
}

void *arraySum(void *input) {
    thread_data_t *td = (thread_data_t *)input;

    while (1) {
        long long int threadSum = 0;
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < td->numVals; i++) {
            threadSum += td->data[i];
        }
        clock_gettime(CLOCK_MONOTONIC, &end);

        long latency = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
        print_progress(td->localTid, latency);

        pthread_mutex_lock(td->lock);
        *(td->totalSum) += threadSum;
        pthread_mutex_unlock(td->lock);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        return -1;
    }

    TERM_CLEAR();
    
    int num_threads = atoi(argv[1]);
    
    int *data = (int *)malloc(sizeof(int) * ARR_SIZE);

    long long int totalSum = 0;

    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    thread_data_t td_arr[num_threads];
    for (int i = 0; i < num_threads; i++) {
        td_arr[i].localTid = i;
        td_arr[i].data = data;
        td_arr[i].numVals = ARR_SIZE;
        td_arr[i].lock = &lock;
        td_arr[i].totalSum = &totalSum;
    }

    pthread_t threads[num_threads];

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, arraySum, &td_arr[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    free(data);
    return 0;
}
