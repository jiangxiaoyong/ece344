#ifndef _SEGMENTATION_H_
#define _SEGMENTATION_H_

#include <types.h>

void segmentation_handler(vaddr_t faultaddress, 
		          int faulttype,
			  vaddr_t heap_start,
			  vaddr_t stacktop,
			  vaddr_t segment1_top,
			  vaddr_t segment1_bottom,
			  vaddr_t segment2_top,
			  vaddr_t segment2_bottom
			  
			  );
#endif
