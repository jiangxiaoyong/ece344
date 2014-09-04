#include <segmentation.h>
#include <tlb_handling.h>
#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <lib.h>
#include <addrspace.h>
#include <vm.h>
#include <machine/spl.h>
#include <machine/tlb.h>
#include <thread.h>
#include <curthread.h>
#include <coremap.h>
#include <addrspace.h>
#include <pagetable.h>
#include <process.h>

void segmentation_handler(vaddr_t faultaddress, 
		          int faulttype,
			  vaddr_t heap_start,
			  vaddr_t stacktop,
			  vaddr_t segment1_top,
			  vaddr_t segment1_bottom,
			  vaddr_t segment2_top,
			  vaddr_t segment2_bottom
			  
			  )
{
       /* check the faultaddress within valid region*/
	if((faultaddress >= segment1_bottom && faultaddress <= segment1_top))
	{
	       /* check protection fault*/
		if( faulttype == VM_FAULT_READONLY )
		{
	      		 /*segmentation fault*/	
			freePid(curthread->pid);
			thread_exit();	
		}
	
	}
	else if((faultaddress >= segment2_bottom && faultaddress <= segment2_top))
	{
	       /* check protection fault*/
		if( faulttype == VM_FAULT_READONLY )
		{
	      		 /*segmentation fault*/	
			freePid(curthread->pid);
			thread_exit();	
		}
	}
	/*
	 *temporary work, did not consider malloc heap region
	 */
	else if((faultaddress >= heap_start && faultaddress <= stacktop))
	{
	       /* check protection fault*/
		if( faulttype == VM_FAULT_READONLY )
		{
	      		 /*segmentation fault*/	
			freePid(curthread->pid);
			thread_exit();	
		}
	
	}
	else
	{
      		 /*not in valid region , segmentation fault*/	
		freePid(curthread->pid);
		thread_exit();
	}



}
