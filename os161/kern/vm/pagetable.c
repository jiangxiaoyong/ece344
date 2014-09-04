#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <pagetable.h>
#include <elf.h>
#include <vm.h>
#include <coremap.h>

struct page_directory_entry *
page_table_initializer(void)
{
	int i;
	struct page_directory_entry *pde;

       /* allocate space for first level page table, 1024 entries*/
	pde = kmalloc(PAGE_TABLE_ENTRY_NUM * sizeof(struct page_directory_entry));
	if(pde == NULL)
	{
		panic("no enough memeory at alloc for page table first level\n");
	}

       /* initialize first level page table*/
	for( i = 0; i < PAGE_TABLE_ENTRY_NUM; i++ )
	{
		pde[i].addr_page_table = 0;	
		pde[i].exist = 0;
	}
	
	return pde;

}

int map_page_table(u_int32_t p_addr,
	       	u_int32_t vaddr_vm_fault,
	       	u_int32_t flag, 
		int alloc_pagetable,
		struct page_directory_entry * pde)
{
	vaddr_t vaddr_first, vaddr_second; 	
	int read, write, execute;
	struct page_table_entry *temp_addr;
	struct page_table_entry *pte; 
	u_int32_t ppn;

       /* get protection flag*/
	read = flag & PF_R;
	write = flag & PF_W;
	execute = flag & PF_X;

       /* extract 10 bit of first and second page table from virtual address*/
	vaddr_first = EXTRACT_PAGE_DIRECTORY(vaddr_vm_fault);
	vaddr_second = EXTRACT_PAGE_TABLE(vaddr_vm_fault) ;


       /* new page_directory_entry, and new page table*/
	if(alloc_pagetable == 1)
	{
	       /* alloc 1024 second level page table entry*/
		pte = kmalloc(PAGE_TABLE_ENTRY_NUM * sizeof(struct page_table_entry));
		if(pte == NULL)
		{
			return ENOMEM;
		}

	}
       /* same page_directory_entry, but old page table, already allocated*/
	else
	{
	
		temp_addr = PADDR_TO_KVADDR(pde[vaddr_first].addr_page_table);	
		pte = temp_addr;
	
	}


       /* set seconde level page table entry*/
	pte[vaddr_second].physical_frame_number = p_addr;
	pte[vaddr_second].global = 0;
	pte[vaddr_second].valid = 1;
	pte[vaddr_second].nocache = 0;
	pte[vaddr_second].exist = 1;
	pte[vaddr_second].swapped = 0;

	if(write != 0)
	{
		pte[vaddr_second].writable = 1;	
	}
	else
	{
		pte[vaddr_second].writable = 0;	
	}

	/*
	 *record info of virtual page address and page table, page directory
	 *for future invalid page table and TLB
	 */
	ppn = PADDR_TO_COREMAP(KVADDR_TO_PADDR(p_addr));
	coremap[ppn].vaddr = vaddr_vm_fault;
	coremap[ppn].pt_addr = pte;
	coremap[ppn].pd_addr = pde;

       /* set first level page table*/
	pde[vaddr_first].addr_page_table = pte;
	pde[vaddr_first].exist = 1;

	return 0;

}


