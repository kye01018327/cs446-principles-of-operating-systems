#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#include <signal.h>

#include <ucontext.h>

#include <stdatomic.h>

#include "modify_value.h"

#define REPS 100000
#define STACK_SIZE 65536

int counter = 0;

int task_1_finished = 0;
int task_2_finished = 0;

ucontext_t main_context, task1_context, task2_context;
int current_task = 0;

atomic_flag guard = ATOMIC_FLAG_INIT;

void task_1_func() {
    printf("Task 1 started.\n");

    for (int i=0; i<REPS; ++i) {
		

        // implement spin-guard lock()-ing //
        while (atomic_flag_test_and_set(&guard));

        while (rand() % 8 != 0) {}  // short, random delay to prevent loop unrolling

        if (i % 1000 == 0) { printf("Task 1 running.\n"); }  // debug print every 1000 reps

        modify_value_up(&counter);

        atomic_flag_clear(&guard);
        // implement spin-guard unlock()-ing //


    }
    
    printf("Task 1 finished.\n");
    task_1_finished = 1;
}

void task_2_func() {
    printf("Task 2 started.\n");

    for (int i=0; i<REPS; ++i) {
		

        // implement spin-guard lock()-ing //

        while (atomic_flag_test_and_set(&guard));

        while (rand() % 8 != 0) {}  // short, random delay to prevent loop unrolling

        if (i % 1000 == 0) { printf("Task 2 running.\n"); }  // debug print every 1000 reps

        modify_value_down(&counter);

        atomic_flag_clear(&guard);
        // implement spin-guard unlock()-ing //


    }
    
    printf("Task 2 finished.\n");
    task_2_finished = 1;
}

void time_slice_expired_handler(int signal) {
    
    printf("\t\tTIME SLICE EXPIRED\n");

    // implement simple scheduling between tasks //
    if (current_task == 1) {
        if (!task_2_finished) {
            current_task = 2;
            swapcontext(&task1_context, &task2_context);
        }
    }
    else if (current_task == 2) {
        if (!task_1_finished) {
            current_task = 1;
            swapcontext(&task2_context, &task1_context);
        }
    }
    else {
        if (!task_1_finished) {
            current_task = 1;
            swapcontext(&main_context, &task1_context);
        }
        else if (!task_2_finished) {
            current_task = 2;
            swapcontext(&main_context, &task2_context);
        }
    }
}


int main(int argc, char *argv[]){

    srand(time(NULL));  // initialize random number generator

    // implement task1 and task2 ucontext setup //
    getcontext(&task1_context);
    getcontext(&task2_context);

    task1_context.uc_stack.ss_sp = malloc(STACK_SIZE);
    task1_context.uc_stack.ss_size = STACK_SIZE;
    task1_context.uc_link = &main_context;

    task2_context.uc_stack.ss_sp = malloc(STACK_SIZE);
    task2_context.uc_stack.ss_size = STACK_SIZE;
    task2_context.uc_link = &main_context;

    makecontext(&task1_context, task_1_func, 0);
    makecontext(&task2_context, task_2_func, 0);

    // implement interval timer setup //
    signal(SIGALRM, time_slice_expired_handler);
    struct itimerval timer;

    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 1000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 1000;
    setitimer(ITIMER_REAL, &timer, NULL);

    printf("Main started.\n");
	
    while (!task_1_finished || !task_2_finished) {
        printf("\t\tTasks 1 && 2 not both Finished yet - Waiting...\n");
		
		// implement any other task scheduling operations you might need here //
		
        pause();  // pause the main Thread, to wait for delivery of the the next timer-based signal 
    }
	
    printf("Main: Finished. Final counter: %d\n", counter);
 
    return 0;  
}
