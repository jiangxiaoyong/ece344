#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <kern/limits.h>
#include <kern/unistd.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>

int sbrk(unsigned int amount, unsigned int *old_break)
{

	struct addrspace *as;
	int temp1;
	vaddr_t heap_start, heap_end;

	as = curthread->t_vmspace;
	
	heap_start = as->as_vbase2 + as->as_segmentSize2_refine;
	heap_end = heap_start;

	/*
	 *argument checking
	 */
	if(heap_end + amount < heap_start)
	{
		return EINVAL;	
	}

	if(heap_start + amount >= HEAP_TOP)
	{
		return ENOMEM;	
	}

	if(amount == 0)
	{
		*old_break = heap_end;	
	}
	else if(amount % 4 == 0)
	{
		*old_break = heap_end;	
		heap_end += amount;
	
	}
	else if(amount %4 != 0)
	{
	       /* round up to multipla of 4*/
		temp1 = (4 - (amount % 4)) + amount;
		heap_end += temp1;
	}



	return 0;
}
