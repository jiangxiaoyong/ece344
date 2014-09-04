#ifndef _PAGETABLE_H_
#define _PAGETABLE_H_

#define PAGE_TABLE_ENTRY_NUM 1024	

#define SECOND_PAGE_TABLE 0x003ff000
#define FIRST_PAGE_TABLE 0xffc00000

#define EXTRACT_PAGE_TABLE(vaddr) (((vaddr) & SECOND_PAGE_TABLE) >> 12)
#define EXTRACT_PAGE_DIRECTORY(vaddr) (((vaddr) & FIRST_PAGE_TABLE) >> 22)

#include <types.h>

struct page_table_entry{

	unsigned :6;
	unsigned swapped :1;
	unsigned exist :1;
	unsigned global : 1;
	unsigned valid : 1;
	unsigned writable :1;
	unsigned nocache :1;
	
	unsigned physical_frame_number : 20;

};

struct page_directory_entry{
	
	unsigned :11;
	unsigned exist :1;
	unsigned addr_page_table : 20;
};



struct page_directory_entry *page_table_initializer(void);
int map_page_table(paddr_t p_addr,
       	       	   vaddr_t vaddr_vm_fault, 
		   u_int32_t flag, 
		   int alloc_pagetable,
	       	   struct page_directory_entry * pde);

#endif
