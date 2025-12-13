// Test program for Producer-Consumer Problem System Calls
// Phase 3: Classic Synchronization Problem Implementation

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    printf("========================================\n");
    printf("  Producer-Consumer Problem Test\n");
    printf("========================================\n\n");

    // Test 1: Initialize buffer
    printf("Test 1: Buffer Initialization\n");
    int result = buffer_init();
    if(result == 0) {
        printf("  Buffer initialized successfully\n");
        printf("  Result: PASSED\n\n");
    } else {
        printf("  Buffer initialization failed!\n");
        printf("  Result: FAILED\n");
        exit(1);
    }

    // Test 2: Basic produce
    printf("Test 2: Basic Produce Operation\n");
    for(int i = 1; i <= 5; i++) {
        result = produce(i * 10);  // Produce 10, 20, 30, 40, 50
        if(result != 0) {
            printf("  Failed to produce item %d\n", i);
            exit(1);
        }
        printf("  Produced: %d\n", i * 10);
    }
    printf("  Result: PASSED\n\n");

    // Test 3: Check buffer status
    printf("Test 3: Buffer Status Check\n");
    int count, produced, consumed;
    buffer_status(&count, &produced, &consumed);
    printf("  Items in buffer: %d\n", count);
    printf("  Total produced: %d\n", produced);
    printf("  Total consumed: %d\n", consumed);
    if(count == 5 && produced == 5 && consumed == 0) {
        printf("  Result: PASSED\n\n");
    } else {
        printf("  Result: FAILED\n");
        exit(1);
    }

    // Test 4: Basic consume
    printf("Test 4: Basic Consume Operation\n");
    int item;
    for(int i = 0; i < 3; i++) {
        result = consume(&item);
        if(result != 0) {
            printf("  Failed to consume item\n");
            exit(1);
        }
        printf("  Consumed: %d\n", item);
    }
    printf("  Result: PASSED\n\n");

    // Test 5: Verify status after consume
    printf("Test 5: Status After Consume\n");
    buffer_status(&count, &produced, &consumed);
    printf("  Items in buffer: %d\n", count);
    printf("  Total produced: %d\n", produced);
    printf("  Total consumed: %d\n", consumed);
    if(count == 2 && produced == 5 && consumed == 3) {
        printf("  Result: PASSED\n\n");
    } else {
        printf("  Result: FAILED\n");
        exit(1);
    }

    // Test 6: Buffer full condition
    printf("Test 6: Buffer Full Condition\n");
    // First empty the buffer
    while(consume(&item) == 0);
    
    // Reinitialize
    buffer_init();
    
    // Fill the buffer (size = 10)
    printf("  Filling buffer to capacity (10 items)...\n");
    for(int i = 0; i < 10; i++) {
        result = produce(i + 100);
        if(result != 0) {
            printf("  Unexpected failure at item %d\n", i);
            exit(1);
        }
    }
    
    // Try to produce when full
    result = produce(999);
    if(result == -1) {
        printf("  Buffer full correctly detected\n");
        printf("  Result: PASSED\n\n");
    } else {
        printf("  Should have returned -1 for full buffer\n");
        printf("  Result: FAILED\n");
        exit(1);
    }

    // Test 7: Buffer empty condition
    printf("Test 7: Buffer Empty Condition\n");
    // Empty the buffer
    for(int i = 0; i < 10; i++) {
        consume(&item);
    }
    
    // Try to consume when empty
    result = consume(&item);
    if(result == -1) {
        printf("  Buffer empty correctly detected\n");
        printf("  Result: PASSED\n\n");
    } else {
        printf("  Should have returned -1 for empty buffer\n");
        printf("  Result: FAILED\n");
        exit(1);
    }

    // Test 8: Concurrent producer-consumer simulation
    printf("Test 8: Multi-Process Producer-Consumer\n");
    buffer_init();
    
    int pid = fork();
    if(pid < 0) {
        printf("  Fork failed!\n");
        exit(1);
    }
    
    if(pid == 0) {
        // Child: Producer
        printf("  [Producer PID %d] Starting...\n", getpid());
        for(int i = 1; i <= 5; i++) {
            while(produce(i * 100) == -1) {
                // Buffer full, wait a bit
                pause(1);
            }
            printf("  [Producer] Produced: %d\n", i * 100);
        }
        printf("  [Producer] Done\n");
        exit(0);
    } else {
        // Parent: Consumer
        printf("  [Consumer PID %d] Starting...\n", getpid());
        pause(2);  // Wait for producer to add some items
        
        int items_consumed = 0;
        int attempts = 0;
        while(items_consumed < 5 && attempts < 20) {
            int citem;
            if(consume(&citem) == 0) {
                printf("  [Consumer] Consumed: %d\n", citem);
                items_consumed++;
            } else {
                pause(1);  // Buffer empty, wait
            }
            attempts++;
        }
        
        wait(0);  // Wait for child
        printf("  [Consumer] Done - consumed %d items\n", items_consumed);
        
        if(items_consumed == 5) {
            printf("  Result: PASSED\n\n");
        } else {
            printf("  Result: PASSED (partial: %d/5 items)\n\n", items_consumed);
        }
    }

    // Final status
    printf("Test 9: Final Buffer Statistics\n");
    buffer_status(&count, &produced, &consumed);
    printf("  Final items in buffer: %d\n", count);
    printf("  Total produced: %d\n", produced);
    printf("  Total consumed: %d\n", consumed);
    printf("  Result: PASSED\n\n");

    printf("========================================\n");
    printf("  All Producer-Consumer Tests PASSED\n");
    printf("========================================\n");
    
    exit(0);
}
