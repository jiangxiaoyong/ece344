#include <process.h>
#include <types.h>
#include <lib.h>
#include <synch.h>
#include <kern/errno.h>
#include <kern/limits.h>
#include <array.h>
#include <curthread.h>
#include <thread.h>


/*global process management table*/
struct array *pid_table;



struct process *
process_create(int parentid, struct thread * thread)
{
	struct process * process_node = kmalloc(sizeof(struct process))	;
	if(process_node == NULL)
	{
		return NULL;	
	}

	process_node->parent_pid = parentid;
	process_node->exited = 0;
	process_node->exit_code = -1;
	process_node->exit_lock = sem_create("exit_lock", 0 );
	process_node->mythread = thread;	


	return process_node;

}


void
process_exit(struct process *process)
{
	sem_destroy(process->exit_lock);
	process->mythread = NULL;
	kfree(process);

}

void
pid_table_shutdown(void)
{
	array_destroy(pid_table);
	pid_table = NULL;

}

/*
 * initialize a pid_table array
 */
void
process_table_bootstrap()
{
	struct array *new = array_create(); 
	if(new == NULL)
	{
		panic("fail at process_table_bootstrap\n");
	}
	pid_table = new;

}

int
no_hole_in_array(struct array * pid_table)
{

	int i;
	for(i = 0; i < array_getnum(pid_table); i++ )
	{
		if(array_getguy(pid_table, i) == NULL)
		{
			return 1;
		}
	
	}
	return 0;

}


int
findProcessId(struct array * pid_table)
{

	int i;
	for(i = 0; i < array_getnum(pid_table); i++)
	{
		if(array_getguy(pid_table, i) == NULL)
		{
			break;	
		}	
	}
	return i;

}

int
allocatePid(int *childpid, struct thread * thread)
{

	int new_process_id;
	int result;
	struct process * new_process;
       /* check if the process ID exceed the maximum number*/
	if(array_getnum(pid_table) > PID_MAX && !no_hole_in_array(pid_table))
	{
		return EAGAIN;	
	}

       /* find a null space in the pid_table array*/
	new_process_id = findProcessId(pid_table);

       /* if we didnt find a null space in existing pid_table array, add it at the end of array*/
	if(new_process_id == array_getnum(pid_table))
	{
		new_process = process_create(curthread->pid, thread);
		if(new_process == NULL)
		{
			panic("create new process id fail\n");
		}
		result = array_add(pid_table,new_process);
		if(result)
		{
			return result;	
		}
		
		*childpid = new_process_id;
		return 0;
	
	}
       /* find a null space in the pid_table array*/
	else
	{
		new_process = process_create(curthread->pid, thread);
		if(new_process == NULL)
		{
			panic("create new process id fail\n");
		}

		array_setguy(pid_table, new_process_id, new_process);
		*childpid = new_process_id;
		return 0 ;
	
	}

}

int 
freePid(pid_t pid)
{
	if(pid > PID_MAX || pid <= 0 || (pid-1) > array_getnum(pid_table) )
	{
		return EINVAL;	
	}
	
       /* find the specific guy corresponding to pid*/
	struct process * freePid_process;
	freePid_process = array_getguy(pid_table, pid);
	if(freePid_process == NULL)
	{
		panic("fail to free pid");
	
	}
	array_setguy(pid_table, pid, NULL);
	process_exit(freePid_process);

	return 0;

}