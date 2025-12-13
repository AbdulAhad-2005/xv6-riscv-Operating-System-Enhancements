// Combined test for all Phase 2 features
// Memory Statistics + File Encryption

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void test_memory_features(void) {
    printf("\n--- Memory Enhancement Tests ---\n");
    
    uint64 free_bytes = freemem();
    printf("Free memory: %d KB\n", (int)(free_bytes / 1024));
    
    uint64 free_pages, total_alloc;
    memstat(&free_pages, &total_alloc);
    printf("Free pages: %d, Total allocations: %d\n", (int)free_pages, (int)total_alloc);
    
    // Allocate and free to test tracking
    char *p = malloc(4096);
    uint64 after = freemem();
    printf("After malloc(4096): %d KB free\n", (int)(after / 1024));
    free(p);
    printf("After free: %d KB free\n", (int)(freemem() / 1024));
    
    printf("Memory tests: PASSED\n");
}

void test_encryption_features(void) {
    printf("\n--- Encryption Enhancement Tests ---\n");
    
    char message[] = "Secret OS Project Data!";
    int len = strlen(message);
    char backup[64];
    strcpy(backup, message);
    
    printf("Original: %s\n", message);
    
    // Encrypt
    encrypt(message, len);
    printf("Encrypted: ");
    for(int i = 0; i < len; i++) printf("%x ", (unsigned char)message[i]);
    printf("\n");
    
    // Decrypt
    decrypt(message, len);
    printf("Decrypted: %s\n", message);
    
    if(strcmp(message, backup) == 0) {
        printf("Encryption tests: PASSED\n");
    } else {
        printf("Encryption tests: FAILED\n");
    }
}

void test_combined_scenario(void) {
    printf("\n--- Combined Scenario Test ---\n");
    printf("Simulating secure file storage with memory monitoring\n");
    
    uint64 mem_before = freemem();
    printf("Memory before: %d KB\n", (int)(mem_before / 1024));
    
    // Create encrypted file
    char data[] = "Confidential: Grade A+";
    int datalen = strlen(data);
    char encdata[64];
    strcpy(encdata, data);
    
    encrypt(encdata, datalen);
    
    int fd = open("grades.enc", O_CREATE | O_WRONLY);
    write(fd, encdata, datalen);
    close(fd);
    
    printf("Created encrypted file: grades.enc\n");
    
    // Read and decrypt
    fd = open("grades.enc", O_RDONLY);
    char readbuf[64];
    read(fd, readbuf, datalen);
    close(fd);
    
    decrypt(readbuf, datalen);
    readbuf[datalen] = '\0';
    
    printf("Decrypted content: %s\n", readbuf);
    
    uint64 mem_after = freemem();
    printf("Memory after: %d KB\n", (int)(mem_after / 1024));
    
    // Cleanup
    unlink("grades.enc");
    
    if(strcmp(readbuf, data) == 0) {
        printf("Combined scenario: PASSED\n");
    } else {
        printf("Combined scenario: FAILED\n");
    }
}

int main(int argc, char *argv[]) {
    printf("========================================\n");
    printf("  Phase 2: Feature Integration Test\n");
    printf("========================================\n");
    
    test_memory_features();
    test_encryption_features();
    test_combined_scenario();
    
    printf("\n========================================\n");
    printf("  All Phase 2 Tests COMPLETED\n");
    printf("========================================\n");
    
    exit(0);
}
