#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

int main(int argc, char *argv[])
{
  int mutex, empty, full;
  int pid;
  
  printf("Producer-Consumer Test\n");
  printf("Buffer size: %d, Items to produce: %d\n\n", BUFFER_SIZE, NUM_ITEMS);
  
  // Initialize semaphores
  mutex = sem_init(1);      // Mutex for critical section
  empty = sem_init(BUFFER_SIZE);  // Count of empty slots
  full = sem_init(0);       // Count of full slots
  
  if(mutex < 0 || empty < 0 || full < 0) {
    printf("Failed to initialize semaphores\n");
    exit(1);
  }
  
  printf("Semaphores initialized: mutex=%d, empty=%d, full=%d\n", mutex, empty, full);
  
  pid = fork();
  if(pid == 0) {
    // Consumer process
    for(int i = 0; i < NUM_ITEMS; i++) {
      sem_wait(full);   // Wait for a full slot
      sem_wait(mutex);  // Enter critical section
      
      int item = buffer[out];
      out = (out + 1) % BUFFER_SIZE;
      printf("Consumer: consumed item %d\n", item);
      
      sem_post(mutex);  // Exit critical section
      sem_post(empty);  // Signal an empty slot
      
      pause(1);  // Small delay
    }
    exit(0);
  } else {
    // Producer process
    for(int i = 0; i < NUM_ITEMS; i++) {
      sem_wait(empty);  // Wait for an empty slot
      sem_wait(mutex);  // Enter critical section
      
      buffer[in] = i;
      printf("Producer: produced item %d\n", i);
      in = (in + 1) % BUFFER_SIZE;
      
      sem_post(mutex);  // Exit critical section
      sem_post(full);   // Signal a full slot
      
      pause(1);  // Small delay
    }
    
    wait(0);  // Wait for consumer
    
    // Cleanup semaphores
    sem_destroy(mutex);
    sem_destroy(empty);
    sem_destroy(full);
    
    printf("\nProducer-Consumer test completed!\n");
  }
  
  exit(0);
}
