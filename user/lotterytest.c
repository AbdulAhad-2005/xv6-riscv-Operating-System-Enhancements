#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  
  printf("Lottery Scheduler Test\n");
  printf("Creating processes with different ticket counts\n\n");
  
  // Create child processes with different ticket counts
  for(int i = 1; i <= 3; i++) {
    pid = fork();
    if(pid == 0) {
      // Child process
      settickets(i * 10);  // 10, 20, 30 tickets
      printf("Child %d: PID=%d, Tickets=%d\n", i, getpid(), i * 10);
      
      // Do some work
      int count = 0;
      for(int j = 0; j < 100000000; j++) {
        count++;
      }
      
      printf("Child %d: PID=%d completed\n", i, getpid());
      exit(0);
    }
  }
  
  // Wait for all children
  for(int i = 0; i < 3; i++) {
    wait(0);
  }
  
  // Print process info
  struct pstat ps;
  if(getpinfo(&ps) == 0) {
    printf("\nProcess Statistics:\n");
    printf("PID\tInUse\tTickets\tTicks\n");
    for(int i = 0; i < NPROC; i++) {
      if(ps.inuse[i]) {
        printf("%d\t%d\t%d\t%d\n", ps.pid[i], ps.inuse[i], 
               ps.tickets[i], ps.ticks[i]);
      }
    }
  }
  
  printf("\nLottery scheduler test completed!\n");
  exit(0);
}
