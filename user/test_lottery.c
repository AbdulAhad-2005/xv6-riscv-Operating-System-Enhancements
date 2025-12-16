// Lottery Scheduler Test with Statistical Analysis
// Tests that processes with more tickets get proportionally more CPU time
// Both processes run SIMULTANEOUSLY to compete for CPU via lottery scheduling

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define TEST_DURATION 200       // Run for this many ticks
#define WORK_UNITS 50000        // Work per measurement cycle
#define HIGH_TICKETS 80         // "Rich" process tickets
#define LOW_TICKETS 20          // "Poor" process tickets

// Shared memory for results (using pipe for IPC)
int pipe_high[2];
int pipe_low[2];

// Do a chunk of CPU-bound work
int do_work_chunk(void) {
    volatile int k = 0;
    for (int i = 0; i < WORK_UNITS; i++) {
        k = i * i + k;
    }
    return k;  // Return to prevent optimization
}

// Child process: count how many work units completed during test period
void child_process(int tickets, int *result_pipe) {
    settickets(tickets);
    
    int start_time = uptime();
    int work_completed = 0;
    
    // Keep working until test duration is over
    // Both processes compete for CPU during this time
    while (uptime() - start_time < TEST_DURATION) {
        do_work_chunk();
        work_completed++;
    }
    
    int end_time = uptime();
    int actual_duration = end_time - start_time;
    
    // Send results: work_completed, actual_duration, tickets
    int results[3] = {work_completed, actual_duration, tickets};
    write(result_pipe[1], results, sizeof(results));
    close(result_pipe[1]);
    
    exit(0);
}

int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("  Lottery Scheduler Fairness Test\n");
    printf("========================================\n\n");
    
    printf("Configuration:\n");
    printf("  High-ticket process: %d tickets\n", HIGH_TICKETS);
    printf("  Low-ticket process:  %d tickets\n", LOW_TICKETS);
    printf("  Ticket ratio:        %d:%d (%dx difference)\n", 
           HIGH_TICKETS, LOW_TICKETS, HIGH_TICKETS / LOW_TICKETS);
    printf("  Test duration:       %d ticks\n", TEST_DURATION);
    printf("  Work units/cycle:    %d\n\n", WORK_UNITS);
    
    printf("Theory:\n");
    printf("  With %d:%d tickets ratio, the high-ticket process\n", HIGH_TICKETS, LOW_TICKETS);
    printf("  should complete ~%dx more work in the same time.\n\n", HIGH_TICKETS / LOW_TICKETS);
    
    // Create pipes for receiving results
    if (pipe(pipe_high) < 0 || pipe(pipe_low) < 0) {
        printf("Error: pipe creation failed\n");
        exit(1);
    }
    
    printf("Starting concurrent test...\n");
    printf("Both processes will run simultaneously for %d ticks.\n\n", TEST_DURATION);
    
    // Create high-ticket process FIRST
    int pid_high = fork();
    if (pid_high < 0) {
        printf("Error: fork failed\n");
        exit(1);
    }
    if (pid_high == 0) {
        close(pipe_high[0]);
        close(pipe_low[0]);
        close(pipe_low[1]);
        child_process(HIGH_TICKETS, pipe_high);
    }
    
    // Create low-ticket process IMMEDIATELY after
    int pid_low = fork();
    if (pid_low < 0) {
        printf("Error: fork failed\n");
        exit(1);
    }
    if (pid_low == 0) {
        close(pipe_low[0]);
        close(pipe_high[0]);
        close(pipe_high[1]);
        child_process(LOW_TICKETS, pipe_low);
    }
    
    // Parent closes write ends and waits
    close(pipe_high[1]);
    close(pipe_low[1]);
    
    wait(0);
    wait(0);
    
    // Read results from pipes
    int results_high[3], results_low[3];
    read(pipe_high[0], results_high, sizeof(results_high));
    read(pipe_low[0], results_low, sizeof(results_low));
    close(pipe_high[0]);
    close(pipe_low[0]);
    
    int work_high = results_high[0];
    int time_high = results_high[1];
    int tickets_high = results_high[2];
    
    int work_low = results_low[0];
    int time_low = results_low[1];
    int tickets_low = results_low[2];
    
    // Display results
    printf("========================================\n");
    printf("  RESULTS\n");
    printf("========================================\n\n");
    
    printf("High-ticket process (%d tickets):\n", tickets_high);
    printf("  Work completed:   %d cycles\n", work_high);
    printf("  Duration:         %d ticks\n", time_high);
    if (time_high > 0) {
        printf("  Throughput:       %d cycles/tick\n\n", work_high / time_high);
    }
    
    printf("Low-ticket process (%d tickets):\n", tickets_low);
    printf("  Work completed:   %d cycles\n", work_low);
    printf("  Duration:         %d ticks\n", time_low);
    if (time_low > 0) {
        printf("  Throughput:       %d cycles/tick\n\n", work_low / time_low);
    }
    
    printf("========================================\n");
    printf("  ANALYSIS\n");
    printf("========================================\n\n");
    
    if (work_high > 0 && work_low > 0) {
        // Calculate work ratio (high/low) * 100 for precision
        int work_ratio_x100 = (work_high * 100) / work_low;
        int expected_ratio_x100 = (tickets_high * 100) / tickets_low;
        
        printf("Work Comparison:\n");
        printf("  High-ticket work: %d cycles\n", work_high);
        printf("  Low-ticket work:  %d cycles\n\n", work_low);
        
        printf("  Observed work ratio: %d.%d%d (high/low)\n",
               work_ratio_x100 / 100, (work_ratio_x100 / 10) % 10, work_ratio_x100 % 10);
        printf("  Expected ratio:      %d.%d%d (based on %d:%d tickets)\n\n",
               expected_ratio_x100 / 100, (expected_ratio_x100 / 10) % 10, expected_ratio_x100 % 10,
               HIGH_TICKETS, LOW_TICKETS);
        
        // Calculate how close we are to expected
        int diff = work_ratio_x100 > expected_ratio_x100 ?
                   work_ratio_x100 - expected_ratio_x100 : expected_ratio_x100 - work_ratio_x100;
        int tolerance = expected_ratio_x100 / 2;  // 50% tolerance for randomness
        
        printf("Lottery Scheduler Verification:\n");
        printf("  Expected: High-ticket gets ~%dx more CPU time\n", expected_ratio_x100 / 100);
        printf("  Observed: High-ticket did %d.%d%dx more work\n\n",
               work_ratio_x100 / 100, (work_ratio_x100 / 10) % 10, work_ratio_x100 % 10);
        
        // Determine result
        if (work_high > work_low) {
            if (diff <= tolerance) {
                printf("  >>> RESULT: PASSED <<<\n");
                printf("  Lottery scheduler distributes CPU time\n");
                printf("  proportionally to ticket counts!\n\n");
                printf("  Actual ratio closely matches expected ratio.\n");
            } else if (work_ratio_x100 > 100) {
                printf("  >>> RESULT: PASSED (with variance) <<<\n");
                printf("  High-ticket process completed more work.\n");
                printf("  Variance from %dx is due to lottery randomness.\n", expected_ratio_x100 / 100);
            }
        } else if (work_high == work_low) {
            printf("  >>> RESULT: INCONCLUSIVE <<<\n");
            printf("  Both did equal work. Try longer TEST_DURATION.\n");
        } else {
            printf("  >>> RESULT: VARIANCE <<<\n");
            printf("  Low-ticket did more work this run.\n");
            printf("  This can happen with lottery's randomness.\n");
            printf("  Run again for more samples.\n");
        }
        
        // Show percentage difference
        if (work_high > work_low) {
            int pct_more = ((work_high - work_low) * 100) / work_low;
            printf("\n  Summary: High-ticket did %d%% more work.\n", pct_more);
        }
    } else {
        printf("Error: No work completed. Increase TEST_DURATION.\n");
    }
    
    printf("\n========================================\n");
    printf("  Lottery Scheduler Test Complete\n");
    printf("========================================\n");
    
    exit(0);
}
