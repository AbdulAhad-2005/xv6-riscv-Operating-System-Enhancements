# OS Assignment 2 - xv6 Operating System Enhancements

## Project Overview
This project implements various OS enhancements on the xv6-riscv operating system, including:
- **Phase 1**: Lottery Scheduler
- **Phase 2**: Memory and File System Enhancements
- **Phase 3**: Producer-Consumer Synchronization

### System Calls Summary

| # | Name | Phase | Description |
|---|------|-------|-------------|
| 22 | `settickets(n)` | 1 | Set lottery tickets for scheduling |
| 23 | `memstat(&free, &alloc)` | 2 | Get memory statistics |
| 24 | `encrypt(buf, len)` | 2 | XOR encrypt buffer |
| 25 | `decrypt(buf, len)` | 2 | XOR decrypt buffer |
| 26 | `freemem()` | 2 | Get free memory in bytes |
| 27 | `buffer_init()` | 3 | Initialize shared buffer |
| 28 | `produce(item)` | 3 | Add item to producer-consumer buffer |
| 29 | `consume(&item)` | 3 | Remove item from buffer |
| 30 | `buffer_status(&cnt, &prod, &cons)` | 3 | Get buffer statistics |

---

## Quick Start Guide

### Prerequisites (Ubuntu/WSL)

Install the required tools:
```bash
# Update package list
sudo apt update

# Install RISC-V toolchain
sudo apt install gcc-riscv64-linux-gnu

# Install QEMU for RISC-V
sudo apt install qemu-system-misc

# Install build essentials
sudo apt install build-essential git
```

### Verify Installation
```bash
# Check RISC-V GCC
riscv64-linux-gnu-gcc --version

# Check QEMU (should be >= 7.2)
qemu-system-riscv64 --version
```

### Building the Project
```bash
# Clone the repository
git clone https://github.com/AbdulAhad-2005/xv6-riscv-Operating-System-Enhancements.git
cd OS-Assignment-2

# Clean previous builds
make clean

# Build the kernel and user programs
make

# Build filesystem image (includes all test programs)
make fs.img
```

### Running xv6
```bash
# Start xv6 in QEMU
make qemu

# To exit QEMU: Press Ctrl+A, then X
```

### Running Tests
Once xv6 boots, at the shell prompt ($), run:
```bash
# Phase 1 Tests (Lottery Scheduler)
test_edge       # Test edge cases (negative/zero tickets)
test_stress     # Test stress (20 concurrent processes)
test_lottery    # Test lottery scheduling

# Phase 2 Tests (Memory & Encryption)
test_memory     # Test memory statistics syscalls
test_encrypt    # Test file encryption syscalls
test_phase2     # Integration test for all Phase 2 features

# Phase 3 Tests (Producer-Consumer)
test_prodcons   # Test producer-consumer synchronization
```

---

## Phase 1: Lottery Scheduler Implementation (Completed)

### What is Lottery Scheduling?
Lottery scheduling is a probabilistic scheduling algorithm where:
- Each process is assigned "tickets"
- More tickets = higher probability of being scheduled
- A random ticket is drawn, and the process holding that ticket runs

### Files Modified

| File | Description |
|------|-------------|
| `kernel/proc.h` | Added `tickets` field to process structure |
| `kernel/proc.c` | Implemented lottery scheduler in `scheduler()` function |
| `kernel/syscall.h` | Added `SYS_settickets` system call number (22) |
| `kernel/syscall.c` | Registered `sys_settickets` handler |
| `kernel/sysproc.c` | Implemented `sys_settickets()` function |
| `user/user.h` | Added `settickets()` function prototype |
| `user/usys.pl` | Added entry for `settickets` syscall |

### Key Implementation Details

#### 1. Process Structure (`kernel/proc.h`)
```c
struct proc {
  // ... existing fields ...
  int tickets;        // Number of lottery tickets
  // ... other fields ...
};
```

#### 2. Lottery Scheduler (`kernel/proc.c`)
The scheduler:
1. Counts total tickets of all RUNNABLE processes
2. Picks a random winning ticket
3. Finds the process holding that ticket
4. Runs that process

#### 3. System Call (`kernel/sysproc.c`)
```c
uint64 sys_settickets(void) {
  int n;
  argint(0, &n);
  if(n < 1) n = 1;  // Minimum 1 ticket
  myproc()->tickets = n;
  return 0;
}
```

### Test Programs

| Test | Purpose | Expected Result |
|------|---------|-----------------|
| `test_edge` | Tests negative/zero tickets | "Edge Case Test Passed" |
| `test_stress` | Creates 20 processes | "Stress Test Passed" |
| `test_lottery` | Tests ticket-based scheduling | Both processes complete |

---

## Phase 2: Memory and File System Enhancements ✅ Completed

### Features Implemented

#### Memory Enhancement: Statistics Tracking
- **freemem()**: Returns total free physical memory in bytes
- **memstat()**: Returns detailed memory statistics (free pages, allocation count)

#### File System Enhancement: Encryption System
- **encrypt()**: XOR-encrypt a buffer in place
- **decrypt()**: XOR-decrypt a buffer in place

### New System Calls

| Number | Name | Description |
|--------|------|-------------|
| 23 | memstat | Get memory statistics (free pages, allocations) |
| 24 | encrypt | Encrypt a user-space buffer |
| 25 | decrypt | Decrypt a user-space buffer |
| 26 | freemem | Get free memory in bytes |

### Files Modified (Phase 2)

| File | Changes |
|------|---------|
| `kernel/kalloc.c` | Added `nfree`, `nalloc` tracking; `getfreepages()`, `getmemstat()` |
| `kernel/defs.h` | Added function declarations |
| `kernel/syscall.h` | Added syscall numbers 23-26 |
| `kernel/syscall.c` | Registered new syscall handlers |
| `kernel/sysproc.c` | Implemented `sys_freemem`, `sys_memstat`, `sys_encrypt`, `sys_decrypt` |
| `user/user.h` | Added user function prototypes |
| `user/usys.pl` | Added syscall stubs |

### Test Results

```
$ test_memory
=== Memory Statistics Test ===
Test 1: freemem() system call - PASSED
Test 2: memstat() system call - PASSED
Test 3: Memory allocation tracking - PASSED
Test 4: Multiple allocations - PASSED
=== All Memory Tests PASSED ===

$ test_encrypt
=== File Encryption Test ===
Test 1: Basic encrypt/decrypt - PASSED
Test 2: Numeric data encryption - PASSED
Test 3: File encryption simulation - PASSED
Test 4: Edge cases - PASSED
=== All Encryption Tests PASSED ===

$ test_phase2
All Phase 2 Tests COMPLETED - PASSED
```

### Documentation
See [docs/PHASE2_REPORT.md](docs/PHASE2_REPORT.md) for detailed technical documentation.

---

## Phase 3: Producer-Consumer Synchronization ✅ Completed

### What is the Producer-Consumer Problem?
The Producer-Consumer problem is a classic synchronization challenge where:
- **Producers** generate data items and add them to a shared buffer
- **Consumers** remove and process items from the buffer
- The buffer has a fixed capacity (10 items)
- Synchronization prevents race conditions

### New System Calls (Phase 3)

| Number | Name | Description |
|--------|------|-------------|
| 27 | buffer_init | Initialize the shared buffer (must call first) |
| 28 | produce | Add an item to buffer (-1 if full, -2 if not init) |
| 29 | consume | Remove an item from buffer (-1 if empty, -2 if not init) |
| 30 | buffer_status | Get count, total produced, total consumed |

### Files Modified (Phase 3)

| File | Changes |
|------|---------|
| `kernel/syscall.h` | Added syscall numbers 27-30 |
| `kernel/syscall.c` | Registered new syscall handlers |
| `kernel/sysproc.c` | Implemented shared buffer and syscall handlers |
| `user/user.h` | Added user function prototypes |
| `user/usys.pl` | Added syscall stubs |
| `user/test_prodcons.c` | Created comprehensive test program |
| `Makefile` | Added test_prodcons to build |

### Implementation Details

#### Shared Buffer Structure
```c
#define BUFFER_SIZE 10

struct {
  struct spinlock lock;     // Synchronization lock
  int buffer[BUFFER_SIZE];  // Circular buffer
  int count;                // Current items in buffer
  int in;                   // Producer write index
  int out;                  // Consumer read index
  int initialized;          // Initialization flag
  int produced_total;       // Statistics: total items produced
  int consumed_total;       // Statistics: total items consumed
} sharedbuf;
```

#### Usage Example
```c
#include "user/user.h"

int main() {
    int item;
    
    // Initialize buffer (required first)
    buffer_init();
    
    // Producer: add items
    produce(100);
    produce(200);
    produce(300);
    
    // Consumer: remove items
    consume(&item);  // item = 100
    consume(&item);  // item = 200
    
    // Check status
    int count, prod, cons;
    buffer_status(&count, &prod, &cons);
    printf("Items in buffer: %d\n", count);    // 1
    printf("Total produced: %d\n", prod);       // 3
    printf("Total consumed: %d\n", cons);       // 2
    
    exit(0);
}
```

### Test Results (Phase 3)
```
$ test_prodcons
=== Producer-Consumer Test Suite ===

Test 1: Buffer Initialization - PASSED
Test 2: Basic Produce Operation - PASSED
Test 3: Buffer Status Check - PASSED
Test 4: Basic Consume Operation - PASSED
Test 5: Status After Consume - PASSED
Test 6: Buffer Full Condition - PASSED
Test 7: Buffer Empty Condition - PASSED
Test 8: Multi-Process Producer-Consumer - PASSED
Test 9: Final Statistics - PASSED

=== All Producer-Consumer Tests PASSED ===
```

### Documentation
See [docs/FINAL_REPORT.md](docs/FINAL_REPORT.md) for complete technical documentation.

---

## Project Structure

```
OS-Assignment-2/
├── kernel/               # Kernel source code
│   ├── proc.c            # Process management (lottery scheduler)
│   ├── proc.h            # Process structure definitions
│   ├── kalloc.c          # Memory allocator (memory stats tracking)
│   ├── syscall.c         # System call dispatcher
│   ├── syscall.h         # System call numbers (22-30)
│   ├── sysproc.c         # System call implementations (all phases)
│   ├── defs.h            # Kernel function declarations
│   └── ...               # Other kernel files
├── user/                 # User-space programs
│   ├── test_lottery.c    # Phase 1: Lottery test
│   ├── test_stress.c     # Phase 1: Stress test
│   ├── test_edge.c       # Phase 1: Edge case test
│   ├── test_memory.c     # Phase 2: Memory stats test
│   ├── test_encrypt.c    # Phase 2: Encryption test
│   ├── test_phase2.c     # Phase 2: Integration test
│   ├── test_prodcons.c   # Phase 3: Producer-Consumer test
│   └── ...               # Other user programs
├── docs/                 # Documentation
│   ├── PHASE1_REPORT.md  # Phase 1 technical report
│   ├── PHASE2_REPORT.md  # Phase 2 technical report
│   ├── FINAL_REPORT.md   # Complete project report
│   └── INSTALLATION_GUIDE.md  # Installation & user guide
├── mkfs/                 # File system creation tool
├── Makefile              # Build configuration
└── README.md             # This file
```

---

## Troubleshooting

### Common Issues

1. **"Error: Couldn't find a riscv64 version of GCC/binutils"**
   ```bash
   sudo apt install gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
   ```

2. **QEMU version too old**
   ```bash
   # Check version
   qemu-system-riscv64 --version
   # Need >= 7.2, update if necessary
   sudo apt install qemu-system-misc
   ```

3. **Build errors after changes**
   ```bash
   make clean
   make
   ```

---

## Team Members
- Abdul Ahad
- Omar Farooq
- Obaid Satti 
- Hassan Suffyan
- Talha Javed

## Course Information
- Course: Operating Systems

---

## References
- [MIT xv6-riscv](https://github.com/mit-pdos/xv6-riscv)
- [xv6 Book](https://pdos.csail.mit.edu/6.828/2023/xv6/book-riscv-rev3.pdf)
