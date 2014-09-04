#include <swap.h>
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

static struct vnode *swap_vnode;
struct disk_entry * swap_map;

void swap_bootstrap(void)
{

	int retval;
	struct stat a;
//	struct vnode *swap_vnode;

	retval = vfs_open(SWAP_DEVICE, O_RDWR, &swap_vnode);
	if(retval)
	{
		panic("could not open swap device\n")	;
	}

	VOP_STAT(swap_vnode, &a);
	swap_map = bitmapCreate(a.st_size / PAGE_SIZE);
	if(swap_map == NULL)
	{
		panic("could not create swap map\n")	;
	}

	vfs_close(swap_vnode);
}

int swap_out(paddr_t paddr, int swap_index)
{
	int result;
	struct uio u;
//	struct vnode *swap_vnode;

	result = vfs_open(SWAP_DEVICE, O_RDWR, &swap_vnode);
	mk_kuio(&u, paddr, PAGE_SIZE, swap_index * PAGE_SIZE, UIO_WRITE);
	result = VOP_WRITE(swap_vnode, &u);

	if (result) {
		return result;
	}

	vfs_close(swap_vnode);

	return 0;
}

int
swap_pagein( u_int32_t paddr, int rw, u_int32_t mem, u_int32_t file, u_int32_t offset, int swap, int swap_index)
{
	int result;
	int fillamt;
	struct uio u;
	struct addrspace *as;
//	struct vnode *swap_vnode;

	as = curthread->t_vmspace;

	if(swap == 1)
	{
		result = vfs_open(SWAP_DEVICE, O_RDWR, &swap_vnode);
		mk_kuio(&u, paddr, PAGE_SIZE, swap_index * PAGE_SIZE, UIO_READ);
		result = VOP_READ(swap_vnode, &u);

		if (result) {
			return result;
		}

		vfs_close(swap_vnode);
	}
	else 
	{
		/* Open the file. */
		struct vnode *v;
		result = vfs_open(as->as_progname, O_RDONLY, &v);
		if (result) {
			return result;
		}

		mk_kuio_pd(&u, paddr, mem, file, offset, rw);
		if(rw == UIO_READ)
		{
			result = VOP_READ(v, &u)	;
		}
		else
		{
			result = VOP_WRITE(v, &u);
		}

		if (u.uio_resid != 0) {
			/* short read; problem with executable? */
			kprintf("ELF: short read on segment - file truncated?\n");
			return ENOEXEC;
		}

		/* Fill the rest of the memory space (if any) with zeros */
		fillamt = mem - file;
		if (fillamt > 0) {

			u.uio_resid += fillamt;
			u.uio_rw = UIO_READ;
			result = uiomovezeros(fillamt, &u);
		}
				
		if (result==EIO) {
			panic("swap: EIO on swapfile (offset %ld)\n",
			      (long)as->as_offset1);
		}
		else if (result==EINVAL) {
			panic("swap: EINVAL from swapfile (offset %ld)\n",
			      (long)as->as_offset1);
		}
		else if (result) {
			panic("swap: Error %d from swapfile (offset %ld)\n",
			      result, (long)as->as_offset1);
		}

		/* Done with the file now. */
		vfs_close(v);

		return 0;
	
	}




}
