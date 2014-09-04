#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>       

int main(int argc, char **argv)
{

	(void)argc;
	(void)argv;

 	char  *filename = "/testbin/add";
        char  *args[4];
        pid_t  pid;

        args[0] = "add";
        args[1] = "5";
        args[2] = "12";
        args[3] = NULL;

        pid = fork();
        if (pid == 0) execv(filename, argv);

	return 0 ;

}
