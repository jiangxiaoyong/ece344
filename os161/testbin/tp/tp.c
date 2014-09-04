#include<stdio.h>
#include<unistd.h>

int main(int argc, char **argv)
{

	(void)argc;
	(void)argv;
	int one = 1;
	char string[]= "hello";

	printf(" first arg %d second arg = %s\n", one, string);
	
	return 0;

}
