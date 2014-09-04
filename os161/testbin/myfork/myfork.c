#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{

	(void)argc;
	(void)argv;

//	fork();

	int child_pid;
	int child_pid2;
	int x;

 	char  *filename = "/testbin/add";
        char  *args[4];

        args[0] = "add";
        args[1] = "5";
        args[2] = "12";
        args[3] = NULL;


	child_pid = fork();

	if(child_pid == 0)
	{
		printf("|c123456789|c123456789|c123456789|\n");	
		printf("child pid = %d\n", getpid());
	//	execv(filename, args);

	}
	else
	{

		printf(":p987654321:p987654321:p987654321:\n");
		printf("parent pid = %d\n", getpid());

	}
	


	return 0 ;
}

