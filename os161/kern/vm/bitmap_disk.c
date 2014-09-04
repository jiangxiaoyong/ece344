#include <bitmap_disk.h>
#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <coremap.h>
#include <vm.h>
#include <pagetable.h>

static u_int32_t num_bitmap_entries;
static u_int32_t num_bitmap_free;

struct disk_entry * bitmapCreate(u_int32_t nbits)
{
	int i;
	struct disk_entry *b;

	num_bitmap_entries = nbits;
	num_bitmap_free = num_bitmap_entries;

	b = kmalloc( sizeof(struct disk_entry) * nbits );

       /* zero out all content*/
	for( i = 0 ; i < nbits; i++ )
	{
		b[0].exist = 0;	
	}

	return b;

}

int bitmapDiskAllocatePage(void)
{
	int i;
	int candidate;
	
	if(num_bitmap_free == 0)
	{
		panic("out of disk space\n");
	}

	if(num_bitmap_free > 0)
	{
		for( i = 0; i < num_bitmap_entries; i++ )
		{
		
			if(swap_map[i].exist == 1)
			{
				continue;	
			}	
			candidate = i;
			break;
		
		}	

		swap_map[candidate].exist = 1;
		num_bitmap_free --;

		return candidate;
	
	}

}
