#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_settickets(void)
{
  int n;
  argint(0, &n);
  if(n < 1) n = 1; // Minimum 1 ticket

  acquire(&myproc()->lock);
  myproc()->tickets = n;
  release(&myproc()->lock);
  return 0;
}

// ============================================================
// Phase 2: Memory Enhancement System Calls
// ============================================================

// Get memory statistics: returns free memory in bytes
uint64
sys_freemem(void)
{
  return getfreepages() * PGSIZE;
}

// Get detailed memory statistics
// arg0: pointer to store free pages count
// arg1: pointer to store total allocations count
uint64
sys_memstat(void)
{
  uint64 freepages_addr, totalalloc_addr;
  uint64 freepages, totalalloc;
  
  argaddr(0, &freepages_addr);
  argaddr(1, &totalalloc_addr);
  
  getmemstat(&freepages, &totalalloc);
  
  // Copy results to user space
  struct proc *p = myproc();
  if(copyout(p->pagetable, freepages_addr, (char*)&freepages, sizeof(freepages)) < 0)
    return -1;
  if(copyout(p->pagetable, totalalloc_addr, (char*)&totalalloc, sizeof(totalalloc)) < 0)
    return -1;
    
  return 0;
}

// ============================================================
// Phase 2: File System Enhancement - Simple XOR Encryption
// ============================================================

// Simple XOR cipher key
#define ENCRYPT_KEY 0x5A

// Encrypt a buffer in place using XOR cipher
// arg0: user buffer address
// arg1: length of buffer
uint64
sys_encrypt(void)
{
  uint64 addr;
  int len;
  char buf[512];  // Process in chunks
  
  argaddr(0, &addr);
  argint(1, &len);
  
  if(len <= 0 || len > 4096)  // Limit size for safety
    return -1;
  
  struct proc *p = myproc();
  int processed = 0;
  
  while(processed < len) {
    int chunk = len - processed;
    if(chunk > 512) chunk = 512;
    
    // Copy from user space
    if(copyin(p->pagetable, buf, addr + processed, chunk) < 0)
      return -1;
    
    // XOR encrypt each byte
    for(int i = 0; i < chunk; i++) {
      buf[i] = buf[i] ^ ENCRYPT_KEY;
    }
    
    // Copy back to user space
    if(copyout(p->pagetable, addr + processed, buf, chunk) < 0)
      return -1;
    
    processed += chunk;
  }
  
  return len;  // Return number of bytes encrypted
}

// Decrypt a buffer in place (XOR is symmetric)
// arg0: user buffer address
// arg1: length of buffer
uint64
sys_decrypt(void)
{
  // XOR encryption is symmetric - decryption is the same operation
  return sys_encrypt();
}

// ============================================================
// Phase 3: Producer-Consumer Problem Implementation
// ============================================================

// Shared buffer structure for producer-consumer
#define BUFFER_SIZE 10

struct {
  struct spinlock lock;
  int buffer[BUFFER_SIZE];
  int count;           // Number of items in buffer
  int in;              // Next position to produce
  int out;             // Next position to consume
  int initialized;     // Buffer initialization flag
  int produced_total;  // Total items produced
  int consumed_total;  // Total items consumed
} sharedbuf;

// Initialize the shared buffer
uint64
sys_buffer_init(void)
{
  acquire(&sharedbuf.lock);
  
  // Initialize buffer
  for(int i = 0; i < BUFFER_SIZE; i++)
    sharedbuf.buffer[i] = 0;
  
  sharedbuf.count = 0;
  sharedbuf.in = 0;
  sharedbuf.out = 0;
  sharedbuf.produced_total = 0;
  sharedbuf.consumed_total = 0;
  sharedbuf.initialized = 1;
  
  release(&sharedbuf.lock);
  return 0;
}

// Producer: add item to buffer
// arg0: item to produce
// Returns: 0 on success, -1 if buffer full, -2 if not initialized
uint64
sys_produce(void)
{
  int item;
  argint(0, &item);
  
  acquire(&sharedbuf.lock);
  
  if(!sharedbuf.initialized) {
    release(&sharedbuf.lock);
    return -2;  // Not initialized
  }
  
  if(sharedbuf.count >= BUFFER_SIZE) {
    release(&sharedbuf.lock);
    return -1;  // Buffer full
  }
  
  // Add item to buffer
  sharedbuf.buffer[sharedbuf.in] = item;
  sharedbuf.in = (sharedbuf.in + 1) % BUFFER_SIZE;
  sharedbuf.count++;
  sharedbuf.produced_total++;
  
  release(&sharedbuf.lock);
  return 0;
}

// Consumer: remove item from buffer
// arg0: pointer to store consumed item
// Returns: 0 on success, -1 if buffer empty, -2 if not initialized
uint64
sys_consume(void)
{
  uint64 item_addr;
  argaddr(0, &item_addr);
  
  acquire(&sharedbuf.lock);
  
  if(!sharedbuf.initialized) {
    release(&sharedbuf.lock);
    return -2;  // Not initialized
  }
  
  if(sharedbuf.count <= 0) {
    release(&sharedbuf.lock);
    return -1;  // Buffer empty
  }
  
  // Remove item from buffer
  int item = sharedbuf.buffer[sharedbuf.out];
  sharedbuf.out = (sharedbuf.out + 1) % BUFFER_SIZE;
  sharedbuf.count--;
  sharedbuf.consumed_total++;
  
  release(&sharedbuf.lock);
  
  // Copy item to user space
  struct proc *p = myproc();
  if(copyout(p->pagetable, item_addr, (char*)&item, sizeof(item)) < 0)
    return -3;
  
  return 0;
}

// Get buffer status
// arg0: pointer to store count
// arg1: pointer to store produced_total
// arg2: pointer to store consumed_total
uint64
sys_buffer_status(void)
{
  uint64 count_addr, produced_addr, consumed_addr;
  argaddr(0, &count_addr);
  argaddr(1, &produced_addr);
  argaddr(2, &consumed_addr);
  
  acquire(&sharedbuf.lock);
  int count = sharedbuf.count;
  int produced = sharedbuf.produced_total;
  int consumed = sharedbuf.consumed_total;
  release(&sharedbuf.lock);
  
  struct proc *p = myproc();
  if(copyout(p->pagetable, count_addr, (char*)&count, sizeof(count)) < 0)
    return -1;
  if(copyout(p->pagetable, produced_addr, (char*)&produced, sizeof(produced)) < 0)
    return -1;
  if(copyout(p->pagetable, consumed_addr, (char*)&consumed, sizeof(consumed)) < 0)
    return -1;
  
  return 0;
}
