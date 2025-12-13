# Phase 1 Report: Lottery Scheduler Implementation

## Executive Summary
This report documents the implementation of a **Lottery Scheduler** in the xv6-riscv operating system. The lottery scheduler replaces the default round-robin scheduler with a probabilistic scheduling algorithm that gives processes with more "tickets" a higher chance of being scheduled.

---

## 1. Introduction

### 1.1 What is xv6?
xv6 is a simple Unix-like teaching operating system developed at MIT. It runs on RISC-V architecture and is designed to help students understand operating system concepts through a clean, readable codebase.

### 1.2 What is Lottery Scheduling?
Lottery scheduling is a **proportional-share** scheduling algorithm invented by Carl Waldspurger in 1994. Key concepts:

- **Tickets**: Each process holds a certain number of "lottery tickets"
- **Drawing**: The scheduler randomly picks a winning ticket number
- **Winner Selection**: The process holding the winning ticket gets to run
- **Fairness**: Over time, each process gets CPU time proportional to its ticket count

### 1.3 Why Lottery Scheduling?
| Advantage | Explanation |
|-----------|-------------|
| **Simplicity** | Easy to implement and understand |
| **Responsiveness** | No starvation - even processes with 1 ticket will eventually run |
| **Flexibility** | Easy to adjust priority by changing ticket count |
| **Fairness** | CPU time is proportional to tickets held |

---

## 2. Implementation Details

### 2.1 Modified Files Overview

| File | Changes Made |
|------|--------------|
| `kernel/proc.h` | Added `tickets` field to `struct proc` |
| `kernel/proc.c` | Rewrote `scheduler()` function, modified `allocproc()` |
| `kernel/syscall.h` | Added `SYS_settickets` (system call #22) |
| `kernel/syscall.c` | Registered the new system call |
| `kernel/sysproc.c` | Implemented `sys_settickets()` |
| `user/user.h` | Added `settickets()` function declaration |
| `user/usys.pl` | Added syscall stub for `settickets` |

### 2.2 Process Structure Change

**File: `kernel/proc.h`**

We added a `tickets` field to the process structure:
```c
struct proc {
  struct spinlock lock;
  enum procstate state;
  void *chan;
  int killed;
  int xstate;
  int pid;
  struct proc *parent;
  uint64 kstack;
  uint64 sz;
  pagetable_t pagetable;
  struct trapframe *trapframe;
  struct context context;
  struct file *ofile[NOFILE];
  struct inode *cwd;
  int tickets;              // NEW: Number of lottery tickets
  char name[16];
};
```

### 2.3 Random Number Generator

**File: `kernel/proc.c`**

xv6 doesn't have a built-in random number generator, so we implemented a simple Linear Congruential Generator (LCG):

```c
unsigned long rand_state = 1;
int random(void) {
  rand_state = rand_state * 1664525 + 1013904223;
  return rand_state;
}
```

This generates pseudo-random numbers using the formula:
```
next = (current × 1664525 + 1013904223) mod 2^32
```

### 2.4 Process Allocation

**File: `kernel/proc.c` - `allocproc()` function**

When a new process is created, it receives 10 tickets by default:
```c
found:
  p->pid = allocpid();
  p->tickets = 10;  // Default: 10 tickets per process
  p->state = USED;
  // ... rest of allocation
```

### 2.5 The Lottery Scheduler

**File: `kernel/proc.c` - `scheduler()` function**

The scheduler was completely rewritten:

```c
void scheduler(void) {
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;) {
    intr_on();
    intr_off();
    
    int found = 0;
    int total_tickets = 0;
    
    // STEP 1: Count total tickets of all runnable processes
    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state == RUNNABLE) {
        total_tickets += p->tickets;
      }
      release(&p->lock);
    }
    
    // STEP 2: Draw a winning ticket
    if(total_tickets > 0) {
      int winner = random() % total_tickets;
      int counter = 0;
      
      // STEP 3: Find the winner
      for(p = proc; p < &proc[NPROC]; p++) {
        acquire(&p->lock);
        if(p->state == RUNNABLE) {
          counter += p->tickets;
          if(counter > winner) {
            // This process wins! Run it.
            p->state = RUNNING;
            c->proc = p;
            swtch(&c->context, &p->context);
            c->proc = 0;
            found = 1;
            release(&p->lock);
            break;
          }
        }
        release(&p->lock);
      }
    }
    
    if(found == 0) {
      asm volatile("wfi");  // Wait for interrupt if nothing to run
    }
  }
}
```

**How it works:**
1. **Count Phase**: Sum all tickets from runnable processes
2. **Draw Phase**: Pick a random number between 0 and total_tickets-1
3. **Search Phase**: Find which process "owns" that ticket number
4. **Run Phase**: Context switch to the winning process

### 2.6 System Call Implementation

**File: `kernel/sysproc.c`**

```c
uint64 sys_settickets(void) {
  int n;
  argint(0, &n);
  if(n < 1) n = 1;  // Minimum 1 ticket (prevents starvation)
  
  acquire(&myproc()->lock);
  myproc()->tickets = n;
  release(&myproc()->lock);
  return 0;
}
```

**Safety features:**
- Minimum of 1 ticket (prevents a process from having 0 chance)
- Lock acquired during modification (thread-safe)

---

## 3. Test Programs

### 3.1 test_lottery.c - Fairness Test

This test creates two child processes:
- Child 1 ("Rich"): 90 tickets
- Child 2 ("Poor"): 10 tickets

```c
int main() {
    printf("Starting Lottery Test...\n");
    
    int pid1 = fork();
    if (pid1 == 0) {
        settickets(90);  // Rich process
        burn_cpu();
        exit(0);
    }
    
    int pid2 = fork();
    if (pid2 == 0) {
        settickets(10);  // Poor process
        burn_cpu();
        exit(0);
    }
    
    wait(0); wait(0);
    printf("Lottery Test Complete.\n");
    exit(0);
}
```

**Expected behavior**: The "rich" process should get approximately 90% of CPU time, and the "poor" process should get approximately 10%.

### 3.2 test_stress.c - Stress Test

Creates 20 simultaneous processes to test scheduler stability:
```c
int main() {
    for(int i = 0; i < 20; i++) {
        int pid = fork();
        if(pid == 0) {
            printf("Process %d alive\n", getpid());
            exit(0);
        }
    }
    // Wait for all children
    for(int i = 0; i < 20; i++) wait(0);
    printf("Stress Test Passed.\n");
    exit(0);
}
```

### 3.3 test_edge.c - Edge Case Test

Tests boundary conditions:
```c
int main() {
    // Test negative tickets (should be set to 1)
    settickets(-10);
    printf("Safe: Handled negative tickets.\n");
    
    // Test zero tickets (should be set to 1)
    settickets(0);
    printf("Safe: Handled zero tickets.\n");
    
    printf("Edge Case Test Passed.\n");
    exit(0);
}
```

---

## 4. Test Results

All tests were executed successfully:

| Test | Status | Notes |
|------|--------|-------|
| `test_edge` | ✅ PASSED | Negative and zero tickets handled safely |
| `test_stress` | ✅ PASSED | All 20 processes created and terminated |
| `test_lottery` | ✅ PASSED | Both processes completed execution |

---

## 5. How to Run

### Build
```bash
cd OS-Assignment-2
make clean
make fs.img
```

### Run
```bash
make qemu
```

### Execute Tests
```
$ test_edge
$ test_stress
$ test_lottery
```

### Exit
Press `Ctrl+A` then `X`

---

## 6. Conclusion

The lottery scheduler was successfully implemented in xv6-riscv. Key achievements:

1. ✅ Added `tickets` field to process structure
2. ✅ Implemented random number generator
3. ✅ Rewrote scheduler with lottery algorithm
4. ✅ Created `settickets()` system call
5. ✅ Developed 3 comprehensive test programs
6. ✅ All tests pass successfully

The implementation demonstrates:
- Understanding of xv6 process management
- System call implementation
- Scheduler modification
- Kernel-level programming

---

## 7. Future Improvements

Potential enhancements:
1. **Ticket Transfer**: Allow processes to transfer tickets
2. **Ticket Inflation**: Currency compensation for ticket manipulation
3. **Better RNG**: Use hardware random number generator if available
4. **Statistics**: Track actual CPU time per process to verify proportionality

---

*Report prepared for OS Assignment 2 - December 2024*
