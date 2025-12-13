#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Memory Size Test\n");
  
  int initial_size = memsize();
  printf("Initial memory size: %d bytes\n", initial_size);
  
  // Allocate some memory
  char *p = sbrk(4096);
  if(p == (char*)-1) {
    printf("sbrk failed\n");
    exit(1);
  }
  
  int after_alloc = memsize();
  printf("After allocating 4096 bytes: %d bytes\n", after_alloc);
  printf("Difference: %d bytes\n", after_alloc - initial_size);
  
  // Allocate more memory
  p = sbrk(8192);
  if(p == (char*)-1) {
    printf("sbrk failed\n");
    exit(1);
  }
  
  int after_alloc2 = memsize();
  printf("After allocating 8192 more bytes: %d bytes\n", after_alloc2);
  printf("Total difference: %d bytes\n", after_alloc2 - initial_size);
  
  printf("\nMemory size test completed!\n");
  exit(0);
}
