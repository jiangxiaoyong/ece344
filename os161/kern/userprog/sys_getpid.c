#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <curthread.h>
#include <thread.h>


pid_t getpid(int *retpid)
{
	*retpid = curthread->pid;
	return 0 ;

}
