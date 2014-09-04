#ifndef _TLB_HANDLING_H
#define _TLB_HANDLING_h

#include <pagetable.h>

int tlb_handling(
		unsigned int faultaddress,
	       	unsigned int *tlb_entry_lo,
	       	unsigned int memSize,
		unsigned int fileSize,
		unsigned int offset,
		unsigned int permission,
		int rw,
		int from_stack,
		int pages,
		struct page_directory_entry *pde
		);
#endif