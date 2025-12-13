#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Comprehensive System Enhancement Test\n");
  printf("======================================\n\n");
  
  // Test 1: Lottery Scheduler
  printf("TEST 1: Lottery Scheduler\n");
  printf("Setting tickets to 25...\n");
  if(settickets(25) == 0) {
    printf("PASSED: settickets\n");
  } else {
    printf("FAILED: settickets\n");
  }
  
  // Test 2: Process Info
  printf("\nTEST 2: Process Info\n");
  struct pstat ps;
  if(getpinfo(&ps) == 0) {
    printf("PASSED: getpinfo\n");
    int my_pid = getpid();
    for(int i = 0; i < NPROC; i++) {
      if(ps.pid[i] == my_pid) {
        printf("  Current process: PID=%d, Tickets=%d, Ticks=%d\n", 
               ps.pid[i], ps.tickets[i], ps.ticks[i]);
        break;
      }
    }
  } else {
    printf("FAILED: getpinfo\n");
  }
  
  // Test 3: Memory Size
  printf("\nTEST 3: Memory Statistics\n");
  int size1 = memsize();
  char *p = sbrk(1024);
  if(p == (char*)-1) {
    printf("FAILED: sbrk\n");
  }
  int size2 = memsize();
  if(size2 > size1) {
    printf("PASSED: memsize (grew from %d to %d bytes)\n", size1, size2);
  } else {
    printf("FAILED: memsize\n");
  }
  
  // Test 4: Semaphores
  printf("\nTEST 4: Semaphore Operations\n");
  int sem = sem_init(1);
  if(sem >= 0) {
    if(sem_wait(sem) == 0 && sem_post(sem) == 0) {
      printf("PASSED: sem_init, sem_wait, sem_post\n");
    } else {
      printf("FAILED: sem_wait or sem_post\n");
    }
    if(sem_destroy(sem) == 0) {
      printf("PASSED: sem_destroy\n");
    } else {
      printf("FAILED: sem_destroy\n");
    }
  } else {
    printf("FAILED: sem_init\n");
  }
  
  printf("\n======================================\n");
  printf("Comprehensive test completed!\n");
  exit(0);
}
