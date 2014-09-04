#ifndef _COREMAP_H_
#define _COREMAP_H_

#define INVALID_PADDR ((paddr_t)0)

#define COREMAP_TO_PADDR(i)	(((paddr_t)PAGE_SIZE)*((i)+base_coremap_page))
#define PADDR_TO_COREMAP(page)	(((page)/PAGE_SIZE) - base_coremap_page)


struct coremap_entry
{
	unsigned int selfAddr;
	char pageState; /*P -> not free, H -> free*/
	struct addrspace *as;
	unsigned int startAddr;
	unsigned int length;
	unsigned int isKernel;
	unsigned int fixed;
	unsigned int reference;
	unsigned int counter;
	unsigned int dirty;
	unsigned int copy;
	unsigned int vaddr;
	unsigned int pd_addr;
	unsigned int pt_addr;

};

extern struct coremap_entry *coremap;
extern u_int32_t base_coremap_page;
extern u_int32_t num_coremap_entries;
extern u_int32_t num_coremap_free;

void coremap_bootstrap(void);
paddr_t coremapAllocateOneFrame(int n, int Kernel);
paddr_t coremapAllocateMultipleFrame(int n, int Kernel);
void coremapFree(paddr_t frameAddr);
void print_coremap(void);


#endif
