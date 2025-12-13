#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "semaphore.h"

struct semaphore semaphores[MAX_SEMAPHORES];

void
seminit(void)
{
  for(int i = 0; i < MAX_SEMAPHORES; i++) {
    initlock(&semaphores[i].lock, "semaphore");
    semaphores[i].value = 0;
    semaphores[i].allocated = 0;
  }
}

int
sem_init(int value)
{
  for(int i = 0; i < MAX_SEMAPHORES; i++) {
    acquire(&semaphores[i].lock);
    if(!semaphores[i].allocated) {
      semaphores[i].allocated = 1;
      semaphores[i].value = value;
      release(&semaphores[i].lock);
      return i;
    }
    release(&semaphores[i].lock);
  }
  return -1; // No available semaphore
}

int
sem_wait(int sem_id)
{
  if(sem_id < 0 || sem_id >= MAX_SEMAPHORES)
    return -1;
  
  acquire(&semaphores[sem_id].lock);
  if(!semaphores[sem_id].allocated) {
    release(&semaphores[sem_id].lock);
    return -1;
  }
  
  while(semaphores[sem_id].value <= 0) {
    sleep(&semaphores[sem_id], &semaphores[sem_id].lock);
  }
  semaphores[sem_id].value--;
  release(&semaphores[sem_id].lock);
  return 0;
}

int
sem_post(int sem_id)
{
  if(sem_id < 0 || sem_id >= MAX_SEMAPHORES)
    return -1;
  
  acquire(&semaphores[sem_id].lock);
  if(!semaphores[sem_id].allocated) {
    release(&semaphores[sem_id].lock);
    return -1;
  }
  
  semaphores[sem_id].value++;
  wakeup(&semaphores[sem_id]);
  release(&semaphores[sem_id].lock);
  return 0;
}

int
sem_destroy(int sem_id)
{
  if(sem_id < 0 || sem_id >= MAX_SEMAPHORES)
    return -1;
  
  acquire(&semaphores[sem_id].lock);
  if(!semaphores[sem_id].allocated) {
    release(&semaphores[sem_id].lock);
    return -1;
  }
  
  semaphores[sem_id].allocated = 0;
  semaphores[sem_id].value = 0;
  release(&semaphores[sem_id].lock);
  return 0;
}
