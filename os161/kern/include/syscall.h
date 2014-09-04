#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */
#include <types.h>
#include <machine/trapframe.h>

int sys_reboot(int code);
int write(int filehandle, const void *buf, size_t size);
void _exit(int code);
pid_t fork(struct trapframe *tf);
pid_t getpid(int * retpid);
pid_t waitpid(pid_t pid, int * status, int options, int *wait_pid);
int read(int filehandle, void *buf, size_t size);
int execv(const char *prog, char *const *args);
int sbrk(unsigned int amount, unsigned int * old_break);

#endif /* _SYSCALL_H_ */
