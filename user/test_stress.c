// Stress Test for Lottery Scheduler
// Tests scheduler stability under heavy concurrent load

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PROCESSES 20
#define WORK_CYCLES 10000

int main(int argc, char *argv[]) {
    int i, pid;
    int start_time, end_time;
    int created = 0;
    int completed = 0;
    
    printf("========================================\n");
    printf("  Lottery Scheduler Stress Test\n");
    printf("========================================\n\n");
    
    printf("Configuration:\n");
    printf("  Number of processes: %d\n", NUM_PROCESSES);
    printf("  Work cycles each:    %d\n\n", WORK_CYCLES);
    
    printf("Creating %d concurrent processes...\n\n", NUM_PROCESSES);
    
    start_time = uptime();
    
    // Create all child processes
    for (i = 0; i < NUM_PROCESSES; i++) {
        pid = fork();
        if (pid < 0) {
            printf("  Fork failed at process %d!\n", i);
            break;
        }
        if (pid == 0) {
            // Child: set random-ish tickets based on PID
            int my_pid = getpid();
            int my_tickets = (my_pid % 50) + 10;  // 10-60 tickets
            settickets(my_tickets);
            
            // Do some work
            volatile int k = 0;
            for (int j = 0; j < WORK_CYCLES; j++) {
                k = j * j + k;
            }
            
            // Report completion
            printf("  Process %d completed (tickets: %d)\n", my_pid, my_tickets);
            exit(0);
        }
        created++;
    }
    
    printf("\nCreated %d processes, waiting for completion...\n\n", created);
    
    // Wait for all children
    for (i = 0; i < created; i++) {
        int status;
        wait(&status);
        completed++;
    }
    
    end_time = uptime();
    int duration = end_time - start_time;
    
    printf("\n========================================\n");
    printf("  RESULTS\n");
    printf("========================================\n\n");
    
    printf("Summary:\n");
    printf("  Processes created:   %d\n", created);
    printf("  Processes completed: %d\n", completed);
    printf("  Total time:          %d ticks\n", duration);
    if (duration > 0) {
        printf("  Avg time/process:    %d ticks\n", duration / completed);
    }
    printf("\n");
    
    if (completed == NUM_PROCESSES) {
        printf("  >>> RESULT: PASSED <<<\n");
        printf("  All %d processes completed successfully!\n", NUM_PROCESSES);
        printf("  Lottery scheduler handles concurrent load.\n");
    } else if (completed > 0) {
        printf("  >>> RESULT: PARTIAL PASS <<<\n");
        printf("  %d of %d processes completed.\n", completed, NUM_PROCESSES);
    } else {
        printf("  >>> RESULT: FAILED <<<\n");
        printf("  No processes completed!\n");
    }
    
    printf("\n========================================\n");
    printf("  Stress Test Complete\n");
    printf("========================================\n");
    
    exit(0);
}
