#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Settickets Test\n");
  printf("Testing ticket assignment\n\n");
  
  printf("Initial tickets: checking via getpinfo\n");
  struct pstat ps;
  getpinfo(&ps);
  
  int my_pid = getpid();
  for(int i = 0; i < NPROC; i++) {
    if(ps.pid[i] == my_pid) {
      printf("PID %d has %d tickets\n", my_pid, ps.tickets[i]);
      break;
    }
  }
  
  // Set different ticket values
  printf("\nSetting tickets to 50...\n");
  if(settickets(50) < 0) {
    printf("settickets failed\n");
    exit(1);
  }
  
  getpinfo(&ps);
  for(int i = 0; i < NPROC; i++) {
    if(ps.pid[i] == my_pid) {
      printf("PID %d now has %d tickets\n", my_pid, ps.tickets[i]);
      break;
    }
  }
  
  // Try invalid ticket value
  printf("\nTrying to set invalid ticket value (0)...\n");
  if(settickets(0) < 0) {
    printf("Correctly rejected invalid ticket value\n");
  } else {
    printf("ERROR: Accepted invalid ticket value\n");
  }
  
  printf("\nSettickets test completed!\n");
  exit(0);
}
