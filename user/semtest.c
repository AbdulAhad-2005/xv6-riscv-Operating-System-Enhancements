#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Semaphore Operations Test\n\n");
  
  // Test sem_init
  printf("Testing sem_init...\n");
  int sem1 = sem_init(1);
  int sem2 = sem_init(5);
  int sem3 = sem_init(0);
  
  if(sem1 < 0 || sem2 < 0 || sem3 < 0) {
    printf("sem_init failed\n");
    exit(1);
  }
  printf("Created semaphores: sem1=%d, sem2=%d, sem3=%d\n", sem1, sem2, sem3);
  
  // Test sem_wait and sem_post
  printf("\nTesting sem_wait and sem_post...\n");
  if(sem_wait(sem1) < 0) {
    printf("sem_wait failed\n");
    exit(1);
  }
  printf("sem_wait(sem1) successful\n");
  
  if(sem_post(sem1) < 0) {
    printf("sem_post failed\n");
    exit(1);
  }
  printf("sem_post(sem1) successful\n");
  
  // Test multiple wait/post
  printf("\nTesting multiple operations on sem2 (init value 5)...\n");
  for(int i = 0; i < 3; i++) {
    if(sem_wait(sem2) < 0) {
      printf("sem_wait failed on iteration %d\n", i);
      exit(1);
    }
    printf("sem_wait(sem2) iteration %d\n", i);
  }
  
  for(int i = 0; i < 3; i++) {
    if(sem_post(sem2) < 0) {
      printf("sem_post failed on iteration %d\n", i);
      exit(1);
    }
    printf("sem_post(sem2) iteration %d\n", i);
  }
  
  // Test sem_destroy
  printf("\nTesting sem_destroy...\n");
  if(sem_destroy(sem1) < 0) {
    printf("sem_destroy(sem1) failed\n");
    exit(1);
  }
  printf("Destroyed sem1\n");
  
  if(sem_destroy(sem2) < 0) {
    printf("sem_destroy(sem2) failed\n");
    exit(1);
  }
  printf("Destroyed sem2\n");
  
  if(sem_destroy(sem3) < 0) {
    printf("sem_destroy(sem3) failed\n");
    exit(1);
  }
  printf("Destroyed sem3\n");
  
  // Test invalid operations
  printf("\nTesting invalid operations...\n");
  if(sem_wait(sem1) < 0) {
    printf("Correctly rejected wait on destroyed semaphore\n");
  }
  
  if(sem_destroy(99) < 0) {
    printf("Correctly rejected destroy on invalid semaphore\n");
  }
  
  printf("\nSemaphore test completed!\n");
  exit(0);
}
