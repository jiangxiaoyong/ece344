#include <types.h>
#include <kern/errno.h>
#include <lib.h>

int write(int filehandle, const void *buf, size_t size)
{

	kprintf("%c",*(const char *)buf);

	return 0;
}
