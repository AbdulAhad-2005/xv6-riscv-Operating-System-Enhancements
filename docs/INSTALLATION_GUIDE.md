# xv6-riscv Enhanced OS
## Installation & User Guide

---

## Quick Start (5 Minutes)

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install git build-essential qemu-system-misc gcc-riscv64-linux-gnu gdb-multiarch

# macOS (with Homebrew)
brew tap riscv-software-src/riscv
brew install riscv-tools qemu
```

### Build & Run

```bash
# Clone repository
git clone git@github.com:OmarFarooq127/OS-Assignment-2.git
cd OS-Assignment-2

# Build everything
make clean
make qemu

# You're now in xv6! Try the tests:
$ test_lottery
$ test_memory
$ test_prodcons
```

---

## System Call Reference

### Lottery Scheduler

| Function | Description | Example |
|----------|-------------|---------|
| `settickets(n)` | Set n lottery tickets (higher = more CPU) | `settickets(50);` |

```c
#include "user/user.h"

int main() {
    settickets(100);  // High priority
    // CPU-intensive work here
    exit(0);
}
```

### Memory Statistics

| Function | Description | Return |
|----------|-------------|--------|
| `freemem()` | Get free memory in bytes | uint64 |
| `memstat(&free, &alloc)` | Get free pages & total allocations | 0 on success |

```c
#include "user/user.h"

int main() {
    uint64 free, alloc;
    
    printf("Free memory: %d bytes\n", freemem());
    memstat(&free, &alloc);
    printf("Free pages: %d, Allocations: %d\n", free, alloc);
    exit(0);
}
```

### File Encryption

| Function | Description | Return |
|----------|-------------|--------|
| `encrypt(buf, len)` | XOR encrypt buffer | bytes encrypted |
| `decrypt(buf, len)` | XOR decrypt buffer | bytes decrypted |

```c
#include "user/user.h"

int main() {
    char message[] = "Secret Data";
    int len = strlen(message);
    
    encrypt(message, len);   // Now encrypted
    // ... store or transmit ...
    decrypt(message, len);   // Back to plaintext
    
    exit(0);
}
```

### Producer-Consumer Buffer

| Function | Description | Return |
|----------|-------------|--------|
| `buffer_init()` | Initialize shared buffer | 0 on success |
| `produce(item)` | Add item to buffer | 0 success, -1 full |
| `consume(&item)` | Get item from buffer | 0 success, -1 empty |
| `buffer_status(&cnt, &prod, &cons)` | Get statistics | 0 on success |

```c
#include "user/user.h"

// Producer process
void producer() {
    buffer_init();
    for(int i = 1; i <= 10; i++) {
        while(produce(i * 100) == -1) 
            sleep(1);  // Buffer full, retry
        printf("Produced: %d\n", i * 100);
    }
}

// Consumer process
void consumer() {
    int item;
    for(int i = 0; i < 10; i++) {
        while(consume(&item) == -1)
            sleep(1);  // Buffer empty, retry
        printf("Consumed: %d\n", item);
    }
}
```

---

## Building User Programs

### Step 1: Create Your Program

Create `user/myprogram.c`:

```c
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    printf("Hello from xv6!\n");
    
    // Use new syscalls
    settickets(50);
    printf("Free memory: %d bytes\n", freemem());
    
    exit(0);
}
```

### Step 2: Add to Makefile

Edit `Makefile`, add to `UPROGS`:

```makefile
UPROGS=\
    ...
    $U/_myprogram\
```

### Step 3: Build & Run

```bash
make qemu
$ myprogram
Hello from xv6!
Free memory: 133246976 bytes
```

---

## Test Programs

| Program | Tests | Run Command |
|---------|-------|-------------|
| `test_lottery` | Lottery scheduler ticket distribution | `$ test_lottery` |
| `test_stress` | 20 concurrent processes | `$ test_stress` |
| `test_edge` | Invalid ticket values | `$ test_edge` |
| `test_memory` | Memory syscalls | `$ test_memory` |
| `test_encrypt` | Encryption/decryption | `$ test_encrypt` |
| `test_phase2` | Phase 2 integration | `$ test_phase2` |
| `test_prodcons` | Producer-Consumer | `$ test_prodcons` |

---

## Troubleshooting

| Issue | Solution |
|-------|----------|
| `riscv64-linux-gnu-gcc: not found` | Install cross-compiler: `apt install gcc-riscv64-linux-gnu` |
| `qemu-system-riscv64: not found` | Install QEMU: `apt install qemu-system-misc` |
| Build errors | Run `make clean` then `make` |
| `fs.img locked` | Kill existing QEMU: `pkill qemu` |
| System hangs | Exit with `Ctrl+A` then `X` |

## QEMU Controls

| Keys | Action |
|------|--------|
| `Ctrl+A` then `X` | Exit QEMU |
| `Ctrl+A` then `C` | QEMU monitor |
| `Ctrl+P` | List processes (in xv6) |

---

## Project Team

- Abdul Ahad
- Omar Farooq  
- Obaid Satti
- Hassan Suffyan
- Talha Javed

**Course**: Operating Systems | **Date**: December 2024
