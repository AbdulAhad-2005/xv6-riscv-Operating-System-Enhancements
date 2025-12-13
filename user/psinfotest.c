#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  struct pstat ps;
  
  printf("Process Info Test\n");
  printf("Getting information about all processes\n\n");
  
  if(getpinfo(&ps) < 0) {
    printf("getpinfo failed\n");
    exit(1);
  }
  
  printf("PID\tInUse\tTickets\tTicks\tName\n");
  printf("----------------------------------------\n");
  
  int active_procs = 0;
  for(int i = 0; i < NPROC; i++) {
    if(ps.inuse[i]) {
      printf("%d\t%d\t%d\t%d\n", 
             ps.pid[i], ps.inuse[i], ps.tickets[i], ps.ticks[i]);
      active_procs++;
    }
  }
  
  printf("\nTotal active processes: %d\n", active_procs);
  printf("\nProcess info test completed!\n");
  exit(0);
}
