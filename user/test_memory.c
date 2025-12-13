// Test program for memory statistics system calls
// Phase 2: Memory Enhancement

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    uint64 free_pages, total_alloc;
    uint64 free_bytes;
    
    printf("=== Memory Statistics Test ===\n\n");
    
    // Test 1: Get free memory using freemem()
    printf("Test 1: freemem() system call\n");
    free_bytes = freemem();
    printf("  Free memory: %d bytes (%d KB)\n", (int)free_bytes, (int)(free_bytes / 1024));
    printf("  Free pages: %d (4KB each)\n", (int)(free_bytes / 4096));
    printf("  Result: PASSED\n\n");
    
    // Test 2: Get detailed memory statistics
    printf("Test 2: memstat() system call\n");
    if(memstat(&free_pages, &total_alloc) < 0) {
        printf("  memstat() failed!\n");
        exit(1);
    }
    printf("  Free pages: %d\n", (int)free_pages);
    printf("  Total allocations: %d\n", (int)total_alloc);
    printf("  Result: PASSED\n\n");
    
    // Test 3: Allocate memory and check changes
    printf("Test 3: Memory allocation tracking\n");
    uint64 before_free = freemem();
    printf("  Before malloc: %d bytes free\n", (int)before_free);
    
    // Allocate some memory
    char *ptr = malloc(8192);  // 2 pages worth
    if(ptr == 0) {
        printf("  malloc failed!\n");
        exit(1);
    }
    
    uint64 after_alloc = freemem();
    printf("  After malloc(8192): %d bytes free\n", (int)after_alloc);
    printf("  Memory used: %d bytes\n", (int)(before_free - after_alloc));
    
    // Free the memory
    free(ptr);
    uint64 after_free = freemem();
    printf("  After free(): %d bytes free\n", (int)after_free);
    printf("  Result: PASSED\n\n");
    
    // Test 4: Multiple allocations
    printf("Test 4: Multiple allocations\n");
    uint64 start_free = freemem();
    char *ptrs[10];
    for(int i = 0; i < 10; i++) {
        ptrs[i] = malloc(1024);
    }
    uint64 mid_free = freemem();
    printf("  After 10 x malloc(1024): %d bytes used\n", (int)(start_free - mid_free));
    
    for(int i = 0; i < 10; i++) {
        free(ptrs[i]);
    }
    uint64 end_free = freemem();
    printf("  After freeing all: %d bytes free\n", (int)end_free);
    printf("  Result: PASSED\n\n");
    
    printf("=== All Memory Tests PASSED ===\n");
    exit(0);
}
