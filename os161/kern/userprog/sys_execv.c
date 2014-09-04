#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <kern/limits.h>
#include <kern/unistd.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <vm.h>
#include <vfs.h>
#include <test.h>

int execv(const char *prog, char *const *args)
{
	int result;	
	int kargc;
	int i,j;
	int totalStringSpace;
	char **kargv;
	struct vnode *v;
	vaddr_t entrypoint, stackptr;

	if(prog == NULL || args == NULL)
	{
		return EFAULT;	
	}

	char * pathName = kmalloc(PATH_MAX);
	if(pathName == NULL)
	{
		return ENOMEM;	
	}

       /* copy the path name from user space to kernel sapce*/
	result = copyinstr((const_userptr_t)prog, pathName, PATH_MAX, NULL);
	if(result)
	{
		kfree(pathName);
		return result;	
	}

       /* get argc from of user space*/
	kargc = 0;
	while(args[kargc] != NULL)
	{
		kargc++;	
	}
	
       /* copy the address element in argv in user space*/
	kargv = kmalloc(sizeof(userptr_t) * (kargc + 1));
	if(kargv == NULL)
	{
		return ENOMEM;	
	}

	result = copyin((const_userptr_t) args, kargv, sizeof(userptr_t) * (kargc+1) );
	if(result)
	{
		kfree(pathName);
		kfree(kargv);
		return result;	
	}

       /* copy string from argv of user space*/
	for(i = 0 ; i < kargc; i++)
	{

     	       /* if the length of sting less than or equal four, that is just four Bytes*/
		if((strlen(args[i]) + 1) <= 4)
		{
			totalStringSpace = 1;	
		}
      		 /* if the length of the sting including null chracter is multiple of four*/
		else if((strlen(args[i]) + 1) % 4 == 0)
		{
			totalStringSpace =(strlen(args[i]) + 1)/4;
		}
  	        /* if not multiple of four, just round up to nearest integer*/
		else if((strlen(args[i]) + 1) % 4 != 0)
		{
			totalStringSpace =((strlen(args[i]) + 1)/4) + 1;	
		}

		kargv[i] = kmalloc(sizeof( int ) * totalStringSpace);
		if(kargv[i] == NULL)
		{
      			/*free sapce alread allocated*/
			for(j = 0; j < i; j ++)
			{
				kfree(kargv[j])	;
			}
			kfree(kargv);
			kfree(pathName);
			return ENOMEM;	
		}

		result = copyinstr((const_userptr_t)args[i] , kargv[i], strlen(args[i]) + 1, NULL);
		if(result)
		{
	   		/*free sapce alread allocated*/
			for(j = 0; j <= i; j ++)
			{
				kfree(kargv[j])	;
			}
			kfree(kargv);
			kfree(pathName);
			return result;		
		}
	
	}


	/* Open the file. */
	result = vfs_open(pathName, O_RDONLY, &v);
	if (result) {

		/*free sapce alread allocated*/
		for(i = 0; i < kargc; i ++)
		{
			kfree(kargv[i])	;
		}
		kfree(kargv);
		kfree(pathName);
		return result;
	}

       /* destory curretn virtual address space*/
	if(curthread->t_vmspace != NULL)
	{
		as_destroy(curthread->t_vmspace);
		curthread->t_vmspace = NULL;
	}

	assert(curthread->t_vmspace == NULL);

	/* Create a new address space. */
	curthread->t_vmspace = as_create();
	if (curthread->t_vmspace==NULL) {
		vfs_close(v);

		/*free sapce alread allocated*/
		for(i = 0; i < kargc; i ++)
		{
			kfree(kargv[i])	;
		}
		kfree(kargv);
		kfree(pathName);

		return ENOMEM;
	}

	/* Activate it. */
	as_activate(curthread->t_vmspace);

	/* Load the executable. */
	result = load_elf(v, &entrypoint, pathName);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		vfs_close(v);

		/*free sapce alread allocated*/
		for(i = 0; i < kargc; i ++)
		{
			kfree(kargv[i])	;
		}
		kfree(kargv);
		kfree(pathName);

		return result;
	}

	/* Done with the file now. */
	vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(curthread->t_vmspace, &stackptr);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */

		/*free sapce alread allocated*/
		for(i = 0; i < kargc; i ++)
		{
			kfree(kargv[i])	;
		}
		kfree(kargv);
		kfree(pathName);
		return result;
	}
	
	/*
	 * put args on the stack of user space
	 */

	for(i = kargc - 1; i >= 0; i--)
	{
	         /* if the length of sting less than or equal four, that is just four Bytes*/
		if((strlen(kargv[i]) + 1) <= 4)
		{
			totalStringSpace = 1;	
		}
      		 /* if the length of the sting including null chracter is multiple of four*/
		else if((strlen(kargv[i]) + 1) % 4 == 0)
		{
			totalStringSpace =(strlen(kargv[i]) + 1)/4;
		}
  	        /* if not multiple of four, just round up to nearest integer*/
		else if((strlen(kargv[i]) + 1) % 4 != 0)
		{
			totalStringSpace =((strlen(kargv[i]) + 1)/4) + 1;	
		}
		
      		 /* decrement stackptr, and aligned to multiple of four*/
		stackptr = stackptr - totalStringSpace * 4;

      		 /* copy the sting from kernel to user space straring from 0x7fffffff*/
		result = copyoutstr(kargv[i], (userptr_t)stackptr, strlen(kargv[i]) + 1, NULL );
		if (result) {

			/*free sapce alread allocated*/
			for(i = 0; i < kargc; i ++)
			{
				kfree(kargv[i])	;
			}
			kfree(kargv);
			kfree(pathName);
			return result;
		}
		
		/*assign array of pointer with new address of string, which put on stack of user space*/
      		 /*free previouse allocated space at user space*/
		kfree(kargv[i]);
		kargv[i] = (char *)stackptr;

	}

       /* write null with padding at end of kargv*/
	int * writeNull = NULL;
	stackptr = stackptr - 4;
	writeNull = (int *)stackptr;
	*writeNull = 0x00000000;


       /* put new kargv on the stack of userspace, just below args*/
	for(i = kargc - 1; i >= 0; i -- )
	{
		stackptr = stackptr - 4;	
		result = copyout(&kargv[i], (userptr_t)stackptr, sizeof(vaddr_t) );
		if (result) {

			/*free sapce alread allocated*/
			/*
			 *for(i = 0; i < kargc; i ++)
			 *{
			 *        kfree(kargv[i])	;
			 *}
			 */
			kfree(kargv);
			kfree(pathName);
			return result;
		}
	}

	/*free sapce alread allocated*/

	kfree(kargv);
	kfree(pathName);

	/* Warp to user mode. */
	md_usermode(kargc /*argc*/, (userptr_t)stackptr /*userspace addr of argv*/,
		    stackptr, entrypoint);

	/* md_usermode does not return */
	panic("md_usermode returned\n");
	return EINVAL;
}

