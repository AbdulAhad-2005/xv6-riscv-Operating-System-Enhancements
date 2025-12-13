#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int fd;
  char *original = "test_original.txt";
  char *encrypted = "test_encrypted.txt";
  char *decrypted = "test_decrypted.txt";
  char *key = "secret";
  char buf[100];
  
  printf("File Encryption/Decryption Test\n");
  
  // Create original file
  fd = open(original, O_CREATE | O_WRONLY);
  if(fd < 0) {
    printf("Failed to create original file\n");
    exit(1);
  }
  write(fd, "Hello, this is a test message for encryption!\n", 47);
  close(fd);
  printf("Created original file: %s\n", original);
  
  // Encrypt the file
  if(encrypt(original, encrypted, key) < 0) {
    printf("Encryption failed\n");
    exit(1);
  }
  printf("Encrypted file to: %s\n", encrypted);
  
  // Decrypt the file
  if(decrypt(encrypted, decrypted, key) < 0) {
    printf("Decryption failed\n");
    exit(1);
  }
  printf("Decrypted file to: %s\n", decrypted);
  
  // Read and verify decrypted file
  fd = open(decrypted, O_RDONLY);
  if(fd < 0) {
    printf("Failed to open decrypted file\n");
    exit(1);
  }
  int n = read(fd, buf, sizeof(buf));
  close(fd);
  
  printf("\nDecrypted content:\n");
  write(1, buf, n);
  
  // Cleanup
  unlink(original);
  unlink(encrypted);
  unlink(decrypted);
  
  printf("\nEncryption test completed!\n");
  exit(0);
}
