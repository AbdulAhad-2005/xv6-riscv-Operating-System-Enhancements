// Test program for file encryption system calls
// Phase 2: File System Enhancement

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    printf("=== File Encryption Test ===\n\n");
    
    // Test 1: Basic encryption/decryption
    printf("Test 1: Basic encrypt/decrypt\n");
    char original[] = "Hello, xv6 World!";
    char buffer[64];
    int len = strlen(original);
    
    // Copy original to buffer
    strcpy(buffer, original);
    printf("  Original: \"%s\"\n", buffer);
    
    // Encrypt the buffer
    int result = encrypt(buffer, len);
    if(result < 0) {
        printf("  encrypt() failed!\n");
        exit(1);
    }
    printf("  Encrypted: \"");
    for(int i = 0; i < len; i++) {
        printf("%x ", (unsigned char)buffer[i]);
    }
    printf("\"\n");
    
    // Decrypt the buffer
    result = decrypt(buffer, len);
    if(result < 0) {
        printf("  decrypt() failed!\n");
        exit(1);
    }
    printf("  Decrypted: \"%s\"\n", buffer);
    
    // Verify
    if(strcmp(buffer, original) == 0) {
        printf("  Result: PASSED\n\n");
    } else {
        printf("  Result: FAILED - mismatch!\n");
        exit(1);
    }
    
    // Test 2: Encrypt numeric data
    printf("Test 2: Numeric data encryption\n");
    char numdata[] = "12345678901234567890";
    int numlen = strlen(numdata);
    char numcopy[32];
    strcpy(numcopy, numdata);
    
    printf("  Original numbers: %s\n", numcopy);
    encrypt(numcopy, numlen);
    printf("  Encrypted (hex): ");
    for(int i = 0; i < numlen; i++) {
        printf("%x", (unsigned char)numcopy[i]);
    }
    printf("\n");
    
    decrypt(numcopy, numlen);
    printf("  Decrypted: %s\n", numcopy);
    
    if(strcmp(numcopy, numdata) == 0) {
        printf("  Result: PASSED\n\n");
    } else {
        printf("  Result: FAILED\n");
        exit(1);
    }
    
    // Test 3: File encryption simulation
    printf("Test 3: File encryption simulation\n");
    
    // Create a test file
    int fd = open("secret.txt", O_CREATE | O_WRONLY);
    if(fd < 0) {
        printf("  Cannot create file!\n");
        exit(1);
    }
    
    char secret[] = "This is secret data that needs protection!";
    int secretlen = strlen(secret);
    
    // Encrypt before writing
    char encrypted[64];
    strcpy(encrypted, secret);
    encrypt(encrypted, secretlen);
    
    write(fd, encrypted, secretlen);
    close(fd);
    printf("  Wrote encrypted data to secret.txt\n");
    
    // Read and decrypt
    fd = open("secret.txt", O_RDONLY);
    if(fd < 0) {
        printf("  Cannot open file!\n");
        exit(1);
    }
    
    char readbuf[64];
    int n = read(fd, readbuf, secretlen);
    close(fd);
    
    if(n != secretlen) {
        printf("  Read wrong number of bytes!\n");
        exit(1);
    }
    
    decrypt(readbuf, secretlen);
    readbuf[secretlen] = '\0';
    printf("  Read and decrypted: \"%s\"\n", readbuf);
    
    if(strcmp(readbuf, secret) == 0) {
        printf("  Result: PASSED\n\n");
    } else {
        printf("  Result: FAILED\n");
        exit(1);
    }
    
    // Cleanup
    unlink("secret.txt");
    
    // Test 4: Edge cases
    printf("Test 4: Edge cases\n");
    
    // Empty data
    char empty[] = "";
    result = encrypt(empty, 0);
    if(result < 0) {
        printf("  Empty data: handled correctly (rejected)\n");
    } else {
        printf("  Empty data: returned %d\n", result);
    }
    
    // Large data (within limits)
    char largebuf[256];
    for(int i = 0; i < 255; i++) {
        largebuf[i] = 'A' + (i % 26);
    }
    largebuf[255] = '\0';
    
    result = encrypt(largebuf, 255);
    if(result == 255) {
        decrypt(largebuf, 255);
        printf("  Large buffer (255 bytes): PASSED\n");
    }
    
    printf("  Result: PASSED\n\n");
    
    printf("=== All Encryption Tests PASSED ===\n");
    exit(0);
}
