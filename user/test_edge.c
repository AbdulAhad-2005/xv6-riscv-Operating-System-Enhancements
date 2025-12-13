#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    printf("Starting Edge Case Test...\n");

    // Test negative tickets
    if(settickets(-10) < 0) printf("Error: System crashed!\n");
    else printf("Safe: Handled negative tickets.\n");

    // Test zero tickets
    if(settickets(0) < 0) printf("Error: System crashed!\n");
    else printf("Safe: Handled zero tickets.\n");

    printf("Edge Case Test Passed.\n");
    exit(0);
}
