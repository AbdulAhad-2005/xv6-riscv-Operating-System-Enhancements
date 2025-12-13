// System call numbers
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3
#define SYS_pipe    4
#define SYS_read    5
#define SYS_kill    6
#define SYS_exec    7
#define SYS_fstat   8
#define SYS_chdir   9
#define SYS_dup    10
#define SYS_getpid 11
#define SYS_sbrk   12
#define SYS_pause  13
#define SYS_uptime 14
#define SYS_open   15
#define SYS_write  16
#define SYS_mknod  17
#define SYS_unlink 18
#define SYS_link   19
#define SYS_mkdir  20
#define SYS_close  21
#define SYS_settickets 22
// Phase 2: Memory and File System Enhancements
#define SYS_memstat    23  // Get memory statistics
#define SYS_encrypt    24  // Encrypt file data
#define SYS_decrypt    25  // Decrypt file data
#define SYS_freemem    26  // Get free memory amount
// Phase 3: Producer-Consumer Problem
#define SYS_buffer_init   27  // Initialize shared buffer
#define SYS_produce       28  // Producer: add item to buffer
#define SYS_consume       29  // Consumer: remove item from buffer
#define SYS_buffer_status 30  // Get buffer status
