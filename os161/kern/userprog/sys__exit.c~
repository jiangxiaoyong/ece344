#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <lib.h>
#include <machine/spl.h>
#include <test.h>
#include <synch.h>
#include <thread.h>
#include <scheduler.h>
#include <dev.h>
#include <vfs.h>
#include <vm.h>
#include <syscall.h>
#include <version.h>
#include <curthread.h>
#include <process.h>


void _exit(int exitcode)
{
	struct process *p_node;
	p_node = (struct process *)array_getguy(pid_table, curthread->pid);

       /* write the exit code to process struct */
	p_node->exit_code = exitcode;
	p_node->exited = 1;

	V(p_node->exit_lock);
	

	thread_exit();
}
