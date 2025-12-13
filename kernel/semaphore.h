#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#define MAX_SEMAPHORES 64

struct semaphore {
  struct spinlock lock;
  int value;
  int allocated;
};

#endif // _SEMAPHORE_H_
