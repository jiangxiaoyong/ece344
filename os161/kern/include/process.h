#ifndef _PROCESS_H_
#define _PROCESS_H_

//#include <synch.h>
//#include <thread.h>
#include <types.h>
#include <array.h>


extern struct array *pid_table;

struct process
{

	pid_t parent_pid;
	int exited;
	int exit_code;
	struct semaphore *exit_lock;
       	struct thread *mythread;	

};

struct process *
process_create(int parentid, struct thread * thread);

void
process_exit(struct process *process);

int
allocatePid(int *childpid, struct thread * thread);

void
pid_table_shutdown(void);

void
process_table_bootstrap();

int
no_hole_in_array(struct array * pid_table);


int
findProcessId(struct array * pid_table);

int 
freePid(pid_t pid);

#endif

