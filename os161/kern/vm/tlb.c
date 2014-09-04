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
#include <elf.h>
#include <uio.h>
#include <vfs.h>
#include <vnode.h>
#include <swap.h>
#include <page_replacement.h>

int tlb_handling(
		vaddr_t faultaddress,
	       	u_int32_t *tlb_entry_lo,
	       	u_int32_t memSize,
		u_int32_t fileSize,
		u_int32_t offset,
		u_int32_t permission,
		int rw,
		int from_stack,
		int pages,
		struct page_directory_entry *pde
		)
{
	u_int32_t attribute, temp_addr1, ppn;
	paddr_t paddr;
	
	struct page_table_entry *pte_search;
	struct page_table_entry *temp_addr2;
	vaddr_t vaddr_first, vaddr_second;

	int result;
	int alloc_pagetable = 1;
	int swap = 0;
	int swap_index = 0;

       /* get upper and middle 10 bit of virtual address*/
	vaddr_first = EXTRACT_PAGE_DIRECTORY(faultaddress);
	vaddr_second = EXTRACT_PAGE_TABLE(faultaddress) ;

       /* which specific page within this segment*/
       /* calculate offset*/
	offset = offset + (faultaddress - ( faultaddress & 0xfff00000 )); 

	 /* check if this page in pagetable directory, pagefault otherwise*/
	if(pde[vaddr_first].exist == 1)
	{

		pte_search = pde[vaddr_first].addr_page_table;
		temp_addr1 = pte_search;
		temp_addr2 = PADDR_TO_KVADDR(temp_addr1);
		swap_index = temp_addr2[vaddr_second].physical_frame_number;

		if(temp_addr2[vaddr_second].swapped == 1)
		{
			swap = 1;	
		}
		else
		{
			swap = 0;	
		}

	       /* check if page_table_entry exist of not*/
		if(temp_addr2[vaddr_second].exist == 1)
		{
			pte_search = pde[vaddr_first].addr_page_table;
			temp_addr1 = pte_search;
			temp_addr2 = PADDR_TO_KVADDR(temp_addr1);

			paddr = temp_addr2[vaddr_second].physical_frame_number;
			attribute = temp_addr2[vaddr_second].global | 
				    temp_addr2[vaddr_second].valid << 1 |
				    temp_addr2[vaddr_second].writable << 2|
				    temp_addr2[vaddr_second].nocache  << 3;

			*tlb_entry_lo = paddr | attribute << 8;

			/*
			 *set reference and dirty bit of coremap			
			 */
			ppn = PADDR_TO_COREMAP(paddr);
			coremap[ppn].reference = 1;
			if(rw == UIO_WRITE)
			{
				coremap[ppn].dirty = 1;	
			}

		}
		else//page fault
		{
			alloc_pagetable = 0;
			goto readPage;	
		
		}




	}
	else //we get page fault
	{
		/*get a page from coremap*/
		
readPage:	paddr = PADDR_TO_KVADDR(getppages(1,0));//NOTE: this is none kernel pages

	       /*indicate no enough physical memory*/
		if(paddr == 0)
		{
			paddr = evictPage();
		}

	       /* set reference and dirty bit of coremap*/	
		ppn = PADDR_TO_COREMAP(KVADDR_TO_PADDR(paddr));
		coremap[ppn].reference = 1;
		coremap[ppn].dirty = 0;	

		if(from_stack == 0)
		{

			if(pages > 1)
			{
				fileSize = 0x1000;	
				memSize = 0x1000;
			}
		       /*read one page from disk to memory*/
			result = swap_pagein(paddr, rw, memSize, fileSize, offset, swap, swap_index);
			if(result)
			{
				return result;	
			}

			if(permission & PF_W)
			{
				*tlb_entry_lo = KVADDR_TO_PADDR(paddr) | TLBLO_DIRTY| TLBLO_VALID;
				if(alloc_pagetable == 1)
				{
					result = map_page_table(paddr, faultaddress,  permission, 1,pde);
				}
				else
				{
					result = map_page_table(paddr, faultaddress,  permission, 0,pde);

				}


			}
			else
			{
				*tlb_entry_lo = KVADDR_TO_PADDR(paddr) | TLBLO_NO_WRITE| TLBLO_VALID;	
				if(alloc_pagetable == 1)
				{
					result = map_page_table(paddr, faultaddress,  permission, 1,pde);
				}
				else
				{
					result = map_page_table(paddr, faultaddress,  permission, 0,pde);

				}
			}
		}
		else if(from_stack == 1)
		{
			*tlb_entry_lo =	KVADDR_TO_PADDR(paddr ) | TLBLO_DIRTY | TLBLO_VALID;	
			if(alloc_pagetable == 1)
			{
				result = map_page_table(paddr, faultaddress,(unsigned int) 6, 1,pde);		
			}
			else
			{
				result = map_page_table(paddr, faultaddress,(unsigned int) 6, 0,pde);
			}

		}

	
		if(result)
		{
			return result;	
		}
	}

	return 0;

}
