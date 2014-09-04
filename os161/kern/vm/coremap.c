#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <coremap.h>
#include <vm.h>
#include <pagetable.h>


u_int32_t base_coremap_page;
u_int32_t num_coremap_entries;
u_int32_t num_coremap_free;

struct coremap_entry * coremap;

int flag_print;

void coremap_bootstrap(void)
{

	u_int32_t firstpaddr;
	u_int32_t lastpaddr;
	u_int32_t coremapsize;
	u_int32_t numFrame;
	u_int32_t i;

       /* get available physical frame*/
	ram_getsize(&firstpaddr, &lastpaddr);
	numFrame = (lastpaddr - firstpaddr)/ PAGE_SIZE;

       /* get size of coremap*/
	coremapsize = numFrame * sizeof(struct coremap_entry);
	coremapsize = ROUNDUP(coremapsize, PAGE_SIZE);
	assert((coremapsize & PAGE_FRAME) == coremapsize);


       /* steal page for coremap*/
	coremap = (struct coremap_entry *)PADDR_TO_KVADDR(firstpaddr);
	firstpaddr += coremapsize;

       /* set global coremap paremeters*/
	base_coremap_page = firstpaddr / PAGE_SIZE;
	num_coremap_entries = (lastpaddr / PAGE_SIZE) - base_coremap_page;
	num_coremap_free = num_coremap_entries;

	assert(num_coremap_entries + (coremapsize / PAGE_SIZE) == numFrame);

       /* initialize coremap*/
	for(i = 0; i < num_coremap_entries; i++)
	{
		coremap[i].pageState = 'H';
		coremap[i].selfAddr = COREMAP_TO_PADDR(i);
		coremap[i].as = NULL;
		coremap[i].startAddr = 0;
		coremap[i].length = 0;	
		coremap[i].isKernel = 0;
		coremap[i].fixed = 0;
		coremap[i].reference = 0;
		coremap[i].counter = 0;
		coremap[i].dirty = 0;
		coremap[i].copy = 0;
		coremap[i].vaddr = 0;
		coremap[i].pd_addr = 0;
		coremap[i].pt_addr = 0;
	}



}

paddr_t 
coremapAllocateOneFrame(int n, int kernel)
{
/*
 *        if(flag_print == 1)
 *        {
 *                print_coremap();
 *        }
 *
 *
 */
	int i;
	int candidate;

	if(num_coremap_free == 0)
	{
		kprintf("not enough memory Allocate one frame\n");
		return 0;
	}
       /* find one available physical frame*/
	candidate = -1;
	if(num_coremap_free > 0)
	{
	
		for(i = 0; i < (int)num_coremap_entries; i++)
		{
			if(coremap[i].pageState == 'P')	
			{
				continue;	
			}
			candidate = i;
			break;	
		}	
	       /* set paramter of this frame*/
		coremap[candidate].startAddr = coremap[candidate].selfAddr;
		coremap[candidate].length = 1;
		coremap[candidate].pageState = 'P';
		num_coremap_free--;

	       /* we won't swap out kernel pages*/
		if(kernel == 1)
		{
			coremap[candidate].isKernel = 1;	
		}
		else if(kernel == 0)
		{
			coremap[candidate].isKernel = 0;	
		}
	
		if(flag_print == 1)
		{
			print_coremap();
		}
	
		return COREMAP_TO_PADDR(candidate);
	}


}


paddr_t
coremapAllocateMultipleFrame(int n, int kernel)
{
/*
 *        if(flag_print == 1)
 *        {
 *                print_coremap();
 *        }
 *
 *
 */
	int i,j, candidate;
	int numFrameCounter;

	if( n > (int)num_coremap_free )
	{
		kprintf("not enough memory Multiple physical frame\n");
		return 0;
	}

       /* find protential candidate*/	
	for(i = 0; i < (int)num_coremap_entries; i++)
	{
		if(coremap[i].pageState == 'P')
		{
			continue;	
		}	

		candidate = i;
	       /* check the length of the protential candidate*/
		numFrameCounter = 0;
		for(j = candidate + 1; j < (int)num_coremap_entries; j++)
		{
		
			if(coremap[j].pageState == 'H')
			{
				numFrameCounter++;

				if( n == (numFrameCounter + 1 ))
				{
					goto findMultipleFrame;	
				}
				continue;	
			}	
      			 /* length of free frame less than desired num of physical frame*/
			if(coremap[j].pageState == 'P' && n != (numFrameCounter + 1))	
			{
				i = j;	
				break;
			}
		}
	}


findMultipleFrame:
       /* set parameter of coremap to corresponding state*/
	coremap[candidate].startAddr = coremap[candidate].selfAddr;

	num_coremap_free =  num_coremap_free - n;
	for( i = candidate; i < (candidate + n); i++ )
	{
		coremap[i].pageState = 'P';
		coremap[i].length = n;
		if(kernel == 1)
		{
			coremap[i].isKernel = 1;	
		}
		else if(kernel == 0)
		{
			coremap[i].isKernel = 0;	
		}
	}

	if(flag_print == 1)
	{
		print_coremap();
	}

	return COREMAP_TO_PADDR(candidate);
}

void
print_coremap(void)
{
	int i;
	kprintf("\n\n");
	for(i = 0 ; i < (int)num_coremap_entries; i++)
	{
		kprintf("coremap[%d] selfAddr = %x, pageState = %x, startAddr= %x, length = %d kernel = %d\n ", i, coremap[i].selfAddr, coremap[i].pageState, coremap[i].startAddr, coremap[i].length, coremap[i].isKernel);
	}

}

void coremapFree(paddr_t page)
{
	u_int32_t ppn;
	int i, j;
	char * phy_ptr;

	phy_ptr = (char *)PADDR_TO_KVADDR(page);
/*
 *
 *        flag_print = 1;
 *        print_coremap();
 *
 */
	ppn = PADDR_TO_COREMAP(page);
	assert(ppn < num_coremap_entries);

       /* free one physical page*/
	if(coremap[ppn].length == 1)
	{
		assert(page == coremap[ppn].startAddr);
		coremap[ppn].as = NULL;
		coremap[ppn].pageState = 'H';
		coremap[ppn].startAddr = 0;
		coremap[ppn].length = 0;
		coremap[ppn].isKernel = 0;
		coremap[ppn].reference = 0;
		coremap[ppn].counter = 0;
		coremap[ppn].dirty = 0;
		coremap[ppn].copy = 0;
		coremap[ppn].vaddr = 0;
		coremap[ppn].pd_addr = 0;
		coremap[ppn].pt_addr = 0;
		num_coremap_free++;

	       /* up content in physical memory*/
		for( i = 0; i < PAGE_SIZE; i++ )
		{
			phy_ptr[i] = 0;	
		
		}
	}
       /* free multiple pages*/
	if(coremap[ppn].length > 1)
	{
		assert(page == coremap[ppn].startAddr);
		num_coremap_free += coremap[ppn].length;
		int length;
		length = ppn + coremap[ppn].length;

		for(i = ppn; i < length; i++)
		{
			coremap[i].as = NULL;
			coremap[i].pageState = 'H';
			coremap[i].startAddr = 0;
			coremap[i].length = 0;			
			coremap[i].isKernel = 0;
			coremap[i].reference = 0;
			coremap[i].counter = 0;
			coremap[i].dirty = 0;
			coremap[i].copy = 0;
			coremap[i].vaddr = 0;
			coremap[i].pd_addr = 0;
			coremap[i].pt_addr = 0;

       /*  clean c ntent of physical memory*/
			for( j = 0; j < PAGE_SIZE; j++ )
			{
				phy_ptr[j] = 0;	
			}
			phy_ptr = phy_ptr + 0x1000;
		}	

	}

//	print_coremap();
}



