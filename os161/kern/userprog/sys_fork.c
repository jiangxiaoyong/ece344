#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <machine/trapframe.h>

extern struct lock * forklock;


int fork(struct trapframe *tf)
{
       /* lock_acquire(forklock);*/
	pid_t child_processID;
	int result;

	result = process_fork("process_fork", tf, &child_processID, md_forkentry, NULL);
	if(result)
	{
		return result;	
	}

	tf->tf_v0 = child_processID;

       /* lock_acquire(forklock);*/
	/*lock_release(forklock);*/

	return 0;

}
