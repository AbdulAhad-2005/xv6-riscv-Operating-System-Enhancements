# xv6-riscv-Operating-System-Enhancements

This project implements comprehensive enhancements to the xv6-riscv operating system, a teaching OS developed at MIT. The enhancements span three phases and add significant functionality to the base OS.

## Overview

The project implements **9 new system calls** across three phases and includes **7 comprehensive test programs** to demonstrate and validate the new features.

## Phase 1: Lottery Scheduler

The lottery scheduler implements a proportional-share scheduling algorithm where processes are assigned "tickets" and the scheduler randomly selects a process based on the proportion of tickets it holds.

### New System Calls

1. **`settickets(int number)`**: Sets the number of tickets for the calling process
   - Returns 0 on success, -1 if number < 1
   
2. **`getpinfo(struct pstat *)`**: Returns information about all processes including their tickets and accumulated ticks
   - Fills in a pstat structure with process information
   - Returns 0 on success, -1 on failure

### Implementation Details

- Modified `proc` structure to add `tickets` and `ticks` fields
- Modified `scheduler()` in proc.c to implement lottery scheduling algorithm
- Modified `clockintr()` in trap.c to track CPU ticks per process
- Default ticket count: 1 ticket per process

### Test Programs

- **lotterytest**: Tests lottery scheduler with multiple processes having different ticket counts
- **tickettest**: Tests settickets system call with valid and invalid values
- **psinfotest**: Tests getpinfo system call and displays process statistics

## Phase 2: Memory Statistics & File Encryption

This phase adds memory introspection and file encryption capabilities.

### New System Calls

3. **`memsize(void)`**: Returns the current memory size of the calling process in bytes

4. **`encrypt(char *src, char *dst, char *key)`**: Encrypts a file using XOR cipher
   - src: source file path
   - dst: destination file path
   - key: encryption key
   - Returns 0 on success, -1 on failure

5. **`decrypt(char *src, char *dst, char *key)`**: Decrypts a file using XOR cipher
   - Same parameters as encrypt
   - Returns 0 on success, -1 on failure

### Implementation Details

- `memsize()` returns the process's `sz` field (memory size)
- Encryption/decryption uses simple XOR cipher for educational purposes
- Implemented in sysfile.c as they work with files

### Test Programs

- **memsizetest**: Tests memsize system call by allocating memory and checking size changes
- **encrypttest**: Tests file encryption and decryption, verifies round-trip works correctly

## Phase 3: Producer-Consumer Problem

This phase implements semaphores to solve classic synchronization problems.

### New System Calls

6. **`sem_init(int value)`**: Initializes a semaphore with the given value
   - Returns semaphore ID on success, -1 if no semaphores available

7. **`sem_wait(int sem_id)`**: Performs wait operation on semaphore (P operation)
   - Blocks if semaphore value <= 0
   - Returns 0 on success, -1 on failure

8. **`sem_post(int sem_id)`**: Performs signal operation on semaphore (V operation)
   - Increments semaphore value and wakes waiting processes
   - Returns 0 on success, -1 on failure

9. **`sem_destroy(int sem_id)`**: Destroys a semaphore and frees its resources
   - Returns 0 on success, -1 on failure

### Implementation Details

- Implemented semaphore structure with spinlock for synchronization
- Maximum 64 semaphores system-wide (MAX_SEMAPHORES)
- Semaphores use sleep/wakeup for blocking behavior
- Proper resource management with allocation tracking

### Test Programs

- **semtest**: Tests all semaphore operations (init, wait, post, destroy)
- **prodcons**: Implements classic producer-consumer problem using semaphores
- **testall**: Comprehensive test suite that validates all new features

## Building and Running

### Prerequisites
- RISC-V toolchain (riscv64-linux-gnu-gcc)
- QEMU for RISC-V

### Build
```bash
make TOOLPREFIX=riscv64-linux-gnu-
```

### Run
```bash
make TOOLPREFIX=riscv64-linux-gnu- qemu
```

### Run Tests
Once xv6 boots, run any of the test programs:
```
$ lotterytest
$ memsizetest
$ encrypttest
$ prodcons
$ psinfotest
$ tickettest
$ semtest
$ testall
```

## Key Achievements

✅ **Zero Compilation Errors**: Clean build with all tests compiling successfully

✅ **Full Backward Compatibility**: All original xv6 functionality preserved

✅ **9 New System Calls**: Covering scheduling, memory, encryption, and synchronization

✅ **7 Comprehensive Test Programs**: Thorough validation of all new features

✅ **Production-Quality Code**: Proper error handling, resource management, and documentation

## File Structure

### Kernel Files
- `kernel/pstat.h`: Process statistics structure for getpinfo
- `kernel/semaphore.h/c`: Semaphore implementation
- `kernel/proc.c`: Modified for lottery scheduler
- `kernel/proc.h`: Enhanced process structure
- `kernel/sysproc.c`: System call implementations
- `kernel/sysfile.c`: File-related system calls (encrypt/decrypt)
- `kernel/syscall.h/c`: System call registration
- `kernel/trap.c`: Modified for tick accounting
- `kernel/main.c`: Semaphore initialization

### User Programs
- `user/lotterytest.c`: Lottery scheduler test
- `user/memsizetest.c`: Memory size test
- `user/encrypttest.c`: File encryption test
- `user/prodcons.c`: Producer-consumer test
- `user/psinfotest.c`: Process info test
- `user/tickettest.c`: Ticket assignment test
- `user/semtest.c`: Semaphore operations test
- `user/testall.c`: Comprehensive test suite

## System Call Numbers

```c
#define SYS_settickets  22
#define SYS_getpinfo    23
#define SYS_memsize     24
#define SYS_encrypt     25
#define SYS_decrypt     26
#define SYS_sem_init    27
#define SYS_sem_wait    28
#define SYS_sem_post    29
#define SYS_sem_destroy 30
```

## License

This project extends xv6-riscv which is under MIT License. See LICENSE file for details.

## References

- [xv6-riscv](https://github.com/mit-pdos/xv6-riscv) - Original MIT xv6 for RISC-V
- [xv6 Book](https://pdos.csail.mit.edu/6.828/2023/xv6/book-riscv-rev3.pdf) - xv6 Commentary
