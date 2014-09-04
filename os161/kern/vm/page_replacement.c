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
#include <elf.h>
#include <uio.h>
#include <vfs.h>
#include <vnode.h>
#include <kern/stat.h>
#include <bitmap_disk.h>
#include <page_replacement.h>
#include <swap.h>

void ref_bit_counter(void)
{

	int i;

	for(i = 0; i < (int)num_coremap_entries; i++ )
	{
		if(coremap[i].reference == 0)
		{
			coremap[i].counter ++;		
		}
		else if(coremap[i].reference == 1)
		{
			coremap[i].counter = 0;	
		}

		coremap[i].reference = 0;

	}
	
}

u_int32_t evictPage(void)
{
	int i;
	int evict_index;
	int swap_index;
	int result;
	unsigned int max = 0 ;
	u_int32_t ehi, elo;
	u_int32_t vaddr_first, vaddr_second,  page_table ;
	struct page_directory_entry * page_directory;
	struct page_table_entry * temp_addr;



	/*
	 *find the qualified page to evict
	 */
	for( i = 0; i < (int)num_coremap_entries; i++ )
	{
		/*
		 *skip kernel page, which not gonna swap out
		 */
		if(coremap[i].isKernel == 1)
		{
			continue;	
		}
		if(coremap[i].counter > max)
		{
			max = coremap[i].counter;	
			evict_index = i;
		}	
	
	}

	page_directory = coremap[evict_index].pd_addr;
	page_table = coremap[evict_index].pt_addr;

	vaddr_first = EXTRACT_PAGE_DIRECTORY(coremap[evict_index].vaddr);
	vaddr_second = EXTRACT_PAGE_TABLE(coremap[evict_index].vaddr) ;

	temp_addr = PADDR_TO_KVADDR(page_directory[vaddr_first].addr_page_table);
	/*
	 *if page has been modified or does not have a copy in disk
	 */
	if(coremap[evict_index].dirty == 1 || coremap[evict_index].copy == 0)
	{
		assert(coremap[evict_index].isKernel == 0);

		swap_index = bitmapDiskAllocatePage();
		result = swap_out(PADDR_TO_KVADDR(coremap[evict_index].selfAddr), swap_index);
		if(result)
		{
			panic("fail to swap out page\n");
		}
	
		/*
		 *mark this physical page as copied and swapped out
		 */
		coremap[evict_index].copy = 1;
		temp_addr[vaddr_second].swapped = 1;
	}

	/*
	 *invalidate corresponding page table and TLB
	 */

	ehi = coremap[evict_index].vaddr;
	elo = 0;

	i = TLB_Probe(ehi, elo);
	if(i >= 0)
	{
		TLB_Write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	
	}

	assert((struct page_table_entry *)page_table == temp_addr);

	temp_addr[vaddr_second].valid = 0;
	temp_addr[vaddr_second].exist = 0;
	temp_addr[vaddr_second].physical_frame_number = swap_index;


	/*
	 *mark this physical page as free and return it
	 */
	coremap[evict_index].as = NULL;
	coremap[evict_index].pageState = 'H';
	coremap[evict_index].startAddr = 0;
	coremap[evict_index].length = 0;
	coremap[evict_index].isKernel = 0;
	coremap[evict_index].reference = 0;
	coremap[evict_index].counter = 0;
	coremap[evict_index].dirty = 0;
	coremap[evict_index].copy = 0;
	coremap[evict_index].vaddr = 0;
	coremap[evict_index].pd_addr = 0;
	coremap[evict_index].pt_addr = 0;
	num_coremap_free++;

       /* clean up content in physical memory*/
	char * phy_ptr;
	phy_ptr = (char *)PADDR_TO_KVADDR(COREMAP_TO_PADDR(evict_index));
	for( i = 0; i < PAGE_SIZE; i++ )
	{
		phy_ptr[i] = 0;	
	
	}


	return (unsigned int )phy_ptr;

}
