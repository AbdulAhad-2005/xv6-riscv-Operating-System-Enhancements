#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int i, pid;
    int n = 20; 

    printf("Starting Stress Test (%d processes)...\n", n);

    for(i = 0; i < n; i++) {
        pid = fork();
        if(pid == 0) {
            printf("Process %d alive\n", getpid());
            exit(0);
        }
    }
    for(i = 0; i < n; i++) {
        wait(0);
    }
    printf("Stress Test Passed.\n");
    exit(0);
}
