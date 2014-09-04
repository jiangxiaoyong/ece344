#ifndef _BITMAT_DISK_H_
#define _BITMAP_DISK_H_

#include <types.h>

struct disk_entry
{
	unsigned int exist;

};

/*
 *global swap_map
 */
extern struct disk_entry * swap_map;


/*
 *Function
 */
struct disk_entry * bitmapCreate(u_int32_t nbits);
int bitmapDiskAllocatePage(void);
void bitmapDiskFree();


#endif
