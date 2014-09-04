#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <kern/limits.h>
#include <process.h>
#include <array.h>
#include <curthread.h>
#include <thread.h>
#include <synch.h>


int waitpid(pid_t pid, int * status, int options, int *wait_pid)
{
	struct process * p_node;
	int invalid_status_ptr1;
	int invalid_status_ptr2;
	int result;

	invalid_status_ptr1 = 0x80000000;
	invalid_status_ptr2 = 0x40000000;

       /* check the validity of parameter pid*/
	if(pid > PID_MAX || pid <= 0 || pid > (array_getnum(pid_table) - 1) || options != 0)
	{
		return EINVAL;	
	
	}

	if(status == NULL || status == (int *)invalid_status_ptr1 || status == (int *)invalid_status_ptr2)
	{
		return EFAULT;	
	}

       /* if we didnot find the specific pid value in our pid_table*/
	if(array_getguy(pid_table, pid) == NULL)
	{
		kprintf("process does not exist\n");
		return EINVAL;
	}

       /* parent process can ONLY interested in own child process*/
	if(((struct process *)array_getguy(pid_table, pid))->parent_pid != curthread->pid )
	{
		kprintf("parent process NOT interested in own child process\n")	;
		return EINVAL;
	}



       /* get the process guy with specific pid*/
	p_node = (struct process *)array_getguy(pid_table, pid);
	
       /* check if the child process has exited*/
	P(p_node->exit_lock);

       /* child already really exit*/
	 /*return the exit code*/
	*status = p_node->exit_code;	
	 /*return the child process id*/
	*wait_pid = pid;

	result = freePid(pid);
	if(result)
	{
		return result;	
	}




	return 0;
}