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
#include <segmentation.h>
#include <tlb_handling.h>
#define DUMBVM_STACKPAGES    12
/*
 * Note! If OPT_DUMBVM is set, as is the case until you start the VM
 * assignment, this file is not compiled or linked or in any way
 * used. The cheesy hack versions in dumbvm.c are used instead.
 */

struct coremap_entry *coremap;

void
vm_bootstrap(void)
{
	coremap_bootstrap();

}


paddr_t
getppages(unsigned long npages, int kernel)
{
	int spl;
	paddr_t addr;

	spl = splhigh();
	
	if(npages > 1)
	{
		addr = coremapAllocateMultipleFrame(npages, kernel);
	}
	else
	{
		addr = coremapAllocateOneFrame(1, kernel);
	}
	if( addr == INVALID_PADDR )
	{
		return 0;	
	}
	
	splx(spl);
	return addr;
}


struct addrspace *
as_create(void)
{
	struct addrspace *as = kmalloc(sizeof(struct addrspace));
	if (as==NULL) {
		return NULL;
	}

	/*
	 * Initialize as needed.
	 */
	as->as_vbase1 = 0;
	as->as_pbase1 = 0;
	as->as_npages1 = 0;
	as->as_vbase2 = 0;
	as->as_pbase2 = 0;
	as->as_npages2 = 0;
	as->as_stackpbase = 0;
	as->as_permission1 = 0;
	as->as_permission2 = 0;
	as->as_segmentSize1 = 0;
	as->as_segmentSize2 = 0;
	as->as_segmentSize1_refine = 0;
	as->as_segmentSize2_refine = 0;
	as->as_offset1 = 0;
	as->as_offset2 = 0;
	as->as_filez1 = 0;
	as->as_filez2 = 0;
	as->as_v = NULL;
	as->as_progname = NULL;



	return as;
}

int
as_copy(struct addrspace *old,
	struct addrspace **ret,
 	struct page_directory_entry * old_pd, 
	struct page_directory_entry * new_pd)
{

	struct addrspace *new;
	struct page_table_entry *pte; 
	struct page_table_entry *pte_new;
	int i, j;
	paddr_t find_phy_frame, paddr;

	new = as_create();
	if (new==NULL) {
		return ENOMEM;
	}

	new->as_vbase1 = old->as_vbase1;
	new->as_npages1 = old->as_npages1;
	new->as_segmentSize1 = old->as_segmentSize1;
	new->as_segmentSize1_refine = old->as_segmentSize1_refine;
	new->as_offset1 = old->as_offset1;
	new->as_permission1 = old->as_permission1;
	new->as_filez1 = old->as_filez1;


	new->as_vbase2 = old->as_vbase2;
	new->as_npages2 = old->as_npages2;
	new->as_segmentSize2 = old->as_segmentSize2;
	new->as_segmentSize2_refine = old->as_segmentSize2_refine;
	new->as_offset2 = old->as_offset2;
	new->as_permission2 = old->as_permission2;
	new->as_filez2 = old->as_filez2;

	/*
	 *copy physical frame data and set new page directory and page table for new thread  
	 */

	 /* loop page directory to see any page table*/
	for( i = 0 ; i < PAGE_TABLE_ENTRY_NUM; i ++ )
	{
	
      		 /* find a page table exist*/
		if(old_pd[i].exist == 1)
		{

			 /* allocate new page table for new as of new thread*/
			pte_new = kmalloc(PAGE_TABLE_ENTRY_NUM * sizeof(struct page_table_entry));
			if(pte == NULL)
			{
				return ENOMEM;	
			}
			new_pd[i].addr_page_table = pte_new;
			new_pd[i].exist = old_pd[i].exist;

			pte = PADDR_TO_KVADDR(old_pd[i].addr_page_table);			
			
      			 /*  loop inner page table*/
			for(j = 0; j < PAGE_TABLE_ENTRY_NUM; j ++)
			{
      				 /* find a physical frame, and copy it*/
				if(pte[j].exist == 1)
				{
					find_phy_frame = PADDR_TO_KVADDR(pte[j].physical_frame_number);			
      					 /* copy this physical frame*/
					paddr = kmalloc( sizeof(char) * PAGE_SIZE);
					memmove(paddr, find_phy_frame, PAGE_SIZE);



				       /*set content of page table and page directory, including attribute*/
					pte_new[j].physical_frame_number = paddr;
					pte_new[j].global = pte[j].global;
					pte_new[j].valid = pte[j].valid;
					pte_new[j].nocache = pte[j].nocache;
					pte_new[j].exist = pte[j].exist;
					pte_new[j].writable = pte[j].writable;


				}		
			
			}
		}	

	}


/*
 *        if (as_prepare_load(new)) {
 *                as_destroy(new);
 *                return ENOMEM;
 *        }
 *
 *        assert(new->as_pbase1 != 0);
 *        assert(new->as_pbase2 != 0);
 *        assert(new->as_stackpbase != 0);
 *
 *        memmove((void *)PADDR_TO_KVADDR(new->as_pbase1),
 *                (const void *)PADDR_TO_KVADDR(old->as_pbase1),
 *                old->as_npages1*PAGE_SIZE);
 *
 *        memmove((void *)PADDR_TO_KVADDR(new->as_pbase2),
 *                (const void *)PADDR_TO_KVADDR(old->as_pbase2),
 *                old->as_npages2*PAGE_SIZE);
 *
 *        memmove((void *)PADDR_TO_KVADDR(new->as_stackpbase),
 *                (const void *)PADDR_TO_KVADDR(old->as_stackpbase),
 *                DUMBVM_STACKPAGES*PAGE_SIZE);
 *        
 */
	*ret = new;
	return 0;
}

void
as_destroy(struct addrspace *as)
{
	int i, j;
	struct page_directory_entry *pd;
	struct page_table_entry *pt;
	paddr_t paddr;

	pd = curthread->page_directory_addr;


	for( i = 0; i < PAGE_TABLE_ENTRY_NUM; i++ )
	{
		if(pd[i].exist == 1)
		{
		
			/*
			 *clean up content of physical frame
			 */

      			 /* find page_table address*/
			pt = PADDR_TO_KVADDR(pd[i].addr_page_table);

			for(j = 0; j < PAGE_TABLE_ENTRY_NUM; j++)
			{
			
				if(pt[j].exist == 1)
				{
					paddr = PADDR_TO_KVADDR(pt[j].physical_frame_number);
					kfree(paddr);
	      				 /* clean up content of page table*/
					pt[j].physical_frame_number = 0;
					pt[j].exist = 0;
					pt[j].global = 0;
					pt[j].valid = 0;
					pt[j].writable = 0;
					pt[j].nocache = 0;
				
				}			


			}
 
      			 /* clean up contetn of page directory*/
			pd[i].exist = 0;
			pd[i].addr_page_table = 0;
			kfree(pt);
		
		}	
	
	}

	/*
	 *clean up page directory and page table
	 */
	
	kfree(as);
}

void
as_activate(struct addrspace *as)
{
	/*
	 * Write this.
	 */

	(void)as;  // suppress warning until code gets written
	int i, spl;

	spl = splhigh();

	for (i=0; i<NUM_TLB; i++) {
		TLB_Write(TLBHI_INVALID(i), TLBLO_INVALID(), i);
	}

	splx(spl);
}

/*
 * Set up a segment at virtual address VADDR of size MEMSIZE. The
 * segment in memory extends from VADDR up to (but not including)
 * VADDR+MEMSIZE.
 *
 * The READABLE, WRITEABLE, and EXECUTABLE flags are set if read,
 * write, or execute permission should be set on the segment. At the
 * moment, these are ignored. When you write the VM system, you may
 * want to implement them.
 */
int
as_define_region(struct addrspace *as, vaddr_t vaddr, size_t sz, u_int32_t offset,
		 struct vnode *v, u_int32_t file_size,
		 int protectionFlag, char * progname
		 )
{

	size_t npages; 
	size_t origi_sz = sz;

	/* Align the region. First, the base... */
	sz += vaddr & ~(vaddr_t)PAGE_FRAME;
	vaddr &= PAGE_FRAME;


	/* ...and now the length. */
	sz = (sz + PAGE_SIZE - 1) & PAGE_FRAME;

	npages = sz / PAGE_SIZE;

       /* record vnode for future read from disk to memory*/
	as->as_v = v;
	as->as_progname = kmalloc(sizeof(char) * 20);
	strcpy(as->as_progname, progname);

	if (as->as_vbase1 == 0) {
		as->as_vbase1 = vaddr;
		as->as_npages1 = npages;
		as->as_segmentSize1 = origi_sz;
		as->as_segmentSize1_refine = sz;
		as->as_offset1 = offset;
		as->as_permission1 = protectionFlag;
		as->as_filez1 = file_size;
		return 0;
	}

	if (as->as_vbase2 == 0) {
		as->as_vbase2 = vaddr;
		as->as_npages2 = npages;
		as->as_segmentSize2 = origi_sz;
		as->as_segmentSize2_refine = sz;
		as->as_offset2 = offset;
		as->as_permission2 = protectionFlag;
		as->as_filez2 = file_size;
		return 0;
	}

	/*
	 * Support for more than two regions is not available.
	 */
	kprintf("dumbvm: Warning: too many regions\n");
	return EUNIMP;
}

int
as_prepare_load(struct addrspace *as)
{
	/*
	 * Write this.
	 */

	assert(as->as_pbase1 == 0);
	assert(as->as_pbase2 == 0);
	assert(as->as_stackpbase == 0);

	as->as_pbase1 = getppages((unsigned long)as->as_npages1, TRUE);
	if (as->as_pbase1 == 0) {
		return ENOMEM;
	}

	as->as_pbase2 = getppages((unsigned long)as->as_npages2, TRUE);
	if (as->as_pbase2 == 0) {
		return ENOMEM;
	}

	as->as_stackpbase = getppages((unsigned long)DUMBVM_STACKPAGES, TRUE);
	if (as->as_stackpbase == 0) {
		return ENOMEM;
	}

	return 0;
}

int
as_complete_load(struct addrspace *as)
{
	/*
	 * Write this.
	 */

	(void)as;
	return 0;
}

int
as_define_stack(struct addrspace *as, vaddr_t *stackptr)
{
	/*
	 * Write this.
	 */

	(void)as;

	/* Initial user-level stack pointer */

	*stackptr = USERSTACK;
	return 0;
}

int
vm_fault(int faulttype, vaddr_t faultaddress)
{
	vaddr_t vbase1, vtop1, vbase2, vtop2, stackbase, stacktop;


	int i, result, rw;
	int spl;
	u_int32_t ehi, elo, tlb_hi, tlb_lo; 
	struct addrspace *as;
	struct uio u;

	extern int flag_print;
	flag_print = 1;

	spl = splhigh();

	faultaddress &= PAGE_FRAME;

	DEBUG(DB_VM, "dumbvm: fault: 0x%x\n", faultaddress);

	switch (faulttype) {
	    case VM_FAULT_READONLY:
		rw = UIO_WRITE;
		break;

	    case VM_FAULT_READ:
		rw = UIO_READ;
		break;
	    case VM_FAULT_WRITE:
		rw = UIO_WRITE;
		break;
	    default:
		splx(spl);
		return EINVAL;
	}

	as = curthread->t_vmspace;
	if (as == NULL) {
		/*
		 * No address space set up. This is probably a kernel
		 * fault early in boot. Return EFAULT so as to panic
		 * instead of getting into an infinite faulting loop.
		 */
		return EFAULT;
	}

	/*
	 *print core map
	 */



       /* start of heap*/
	vaddr_t heap_start;
	heap_start = as->as_vbase2 + as->as_segmentSize2_refine ;
	stacktop = USERSTACK;


	vaddr_t segment1_top, segment1_bottom, segment2_top, segment2_bottom;

	segment1_top = as->as_vbase1 + as->as_npages1 * PAGE_SIZE;
	segment2_top = as->as_vbase2 + as->as_npages2 * PAGE_SIZE;

	segment1_bottom = as->as_vbase1;
	segment2_bottom = as->as_vbase2;

       /* check if faultaddress is in valid region, and check protection fault*/
	segmentation_handler(faultaddress, 
			     faulttype, 
			     heap_start, 
			     stacktop,
			     segment1_top,
			     segment1_bottom,
			     segment2_top,
			     segment2_bottom
			     );



       /* check if it is in the region of stack of heap*/
	if(faultaddress >= heap_start && faultaddress < stacktop )
	{
		result = tlb_handling(faultaddress, 
			    &tlb_lo,
			    as->as_segmentSize1,
			    as->as_filez1,
			    as->as_offset1,
			    as->as_permission1,
			    rw,
			    1,
			    as->as_npages1,
			    curthread->page_directory_addr
			    );
	}
       /* lie in segment 1*/
	else if(faultaddress >= as->as_vbase1 && faultaddress < segment1_top)
	{

		result = tlb_handling(faultaddress, 
			    &tlb_lo,
			    as->as_segmentSize1,
			    as->as_filez1,
			    as->as_offset1,
			    as->as_permission1,
			    rw,
			    0,
			    as->as_npages1,
			    curthread->page_directory_addr
			    );

	}
       /* lie in segment 2*/
	else if(faultaddress >= as->as_vbase2 && faultaddress < segment2_top)
	{

		result = tlb_handling(faultaddress, 
			    &tlb_lo,
			    as->as_segmentSize2,
			    as->as_filez2,
			    as->as_offset2,
			    as->as_permission2,
			    rw,
			    0,
			    as->as_npages2,
			    curthread->page_directory_addr
			    );

	}

	if(result)
	{
		return result;	
	}



/*
 *
 *        [> Assert that the address space has been set up properly. <]
 *        assert(as->as_vbase1 != 0);
 *        assert(as->as_pbase1 != 0);
 *        assert(as->as_npages1 != 0);
 *        assert(as->as_vbase2 != 0);
 *        assert(as->as_pbase2 != 0);
 *        assert(as->as_npages2 != 0);
 *        assert(as->as_stackpbase != 0);
 *        assert((as->as_vbase1 & PAGE_FRAME) == as->as_vbase1);
 *        assert((as->as_pbase1 & PAGE_FRAME) == as->as_pbase1);
 *        assert((as->as_vbase2 & PAGE_FRAME) == as->as_vbase2);
 *        assert((as->as_pbase2 & PAGE_FRAME) == as->as_pbase2);
 *        assert((as->as_stackpbase & PAGE_FRAME) == as->as_stackpbase);
 *
 *        vbase1 = as->as_vbase1;
 *        vtop1 = vbase1 + as->as_npages1 * PAGE_SIZE;
 *        vbase2 = as->as_vbase2;
 *        vtop2 = vbase2 + as->as_npages2 * PAGE_SIZE;
 *        stackbase = USERSTACK - DUMBVM_STACKPAGES * PAGE_SIZE;
 *        stacktop = USERSTACK;
 *
 *        if (faultaddress >= vbase1 && faultaddress < vtop1) {
 *                paddr = (faultaddress - vbase1) + as->as_pbase1;
 *        }
 *        else if (faultaddress >= vbase2 && faultaddress < vtop2) {
 *                paddr = (faultaddress - vbase2) + as->as_pbase2;
 *        }
 *        else if (faultaddress >= stackbase && faultaddress < stacktop) {
 *                paddr = (faultaddress - stackbase) + as->as_stackpbase;
 *        }
 *        else {
 *                splx(spl);
 *                return EFAULT;
 *        }
 *
 *
 *        [> make sure it's page-aligned <]
 *        assert((paddr & PAGE_FRAME)==paddr);
 *
 *        for (i=0; i<NUM_TLB; i++) {
 *                TLB_Read(&ehi, &elo, i);
 *                if (elo & TLBLO_VALID) {
 *                        continue;
 *                }
 *                ehi = faultaddress;
 *                elo = paddr | TLBLO_DIRTY | TLBLO_VALID;
 *                DEBUG(DB_VM, "dumbvm: 0x%x -> 0x%x\n", faultaddress, paddr);
 *                TLB_Write(ehi, elo, i);
 *                splx(spl);
 *                return 0;
 *        }
 *        kprintf("dumbvm: Ran out of TLB entries - cannot handle page fault\n");
 *        splx(spl);
 *        return EFAULT;
 *
 */

/*
 *         for (i=0; i<NUM_TLB; i++) {
 *                 TLB_Read(&ehi, &elo, i);
 *                 if (elo & TLBLO_VALID) {
 *                         continue;
 *                 }
 *                 ehi = faultaddress;
 *                 elo = tlb_entry_lo;
 *                 TLB_Write(ehi, elo, i);
 *                 TLB_Read(&ehi, &elo, i);
 *                 splx(spl);
 *                 return 0;
 *         }
 *
 */

	ehi = faultaddress;
	elo = tlb_lo;

	i = TLB_Probe(ehi, elo);
	if( i >= 0 ) // there is a matching entry in TLB
	{
		TLB_Write(ehi, elo, i);
	}
	else // no matching entry in TBL
	{

		TLB_Random(ehi, elo);
	}

	splx(spl);
	return 0 ;

}


/* Allocate/free some kernel-space virtual pages */
vaddr_t 
alloc_kpages(int npages)
{
	paddr_t pa;
	pa = getppages(npages, TRUE);
	if (pa==0) {
		return 0;
	}
	return PADDR_TO_KVADDR(pa);
}

void 
free_kpages(vaddr_t addr)
{
	coremapFree(KVADDR_TO_PADDR(addr));
}

