#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void burn_cpu(int pid, char name) {
    volatile int k = 0; // 'volatile' forces the CPU to actually do the math
    for (int i = 0; i < 100000000; i++) {
        k = i * i; 
    }
    // We print k here just so the compiler knows it was "used"
    printf("Process %d (%c) finished! (Check: %d)\n", pid, name, k);
}
int main(int argc, char *argv[]) {
    int pid1, pid2;

    printf("Starting Lottery Test...\n");

    // Create Child 1 (The "Rich" Process)
    pid1 = fork();
    if (pid1 == 0) {
        settickets(90); // Give 90 tickets
        printf("Child 1 (Rich) started with 90 tickets.\n");
        burn_cpu(getpid(), 'A');
        exit(0);
    }

    // Create Child 2 (The "Poor" Process)
    pid2 = fork();
    if (pid2 == 0) {
        settickets(10); // Give 10 tickets
        printf("Child 2 (Poor) started with 10 tickets.\n");
        burn_cpu(getpid(), 'B');
        exit(0);
    }

    // Parent waits for both to finish
    wait(0);
    wait(0);
    printf("Lottery Test Complete.\n");
    exit(0);
}
