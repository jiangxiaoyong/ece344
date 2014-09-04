#ifndef _KERN_LIMITS_H_
#define _KERN_LIMITS_H_

/* Longest filename (without directory) not including null terminator */
#define NAME_MAX  255

/* Longest full path name */
#define PATH_MAX   1024

/* Max number of PID's */
#define PID_MAX 255
#define HEAP_TOP 0x70000000

#endif /* _KERN_LIMITS_H_ */
