#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <syscall.h>

int read(int filehandle, void *buf, size_t size)
{
	if(buf == NULL)
	{
		return EFAULT;	
	}

	kgets((char *)buf, 2);

	return 0;
}
