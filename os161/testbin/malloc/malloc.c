#include <stdio.h>
#include <stdlib.h>
#include <err.h>

struct simple {

	int first;
	int second;

};

int main( int argc, char **argv )
{

	struct simple *mem;

	mem = malloc(sizeof(struct simple) );
	mem->first = 1;
	mem->second = 2;

	printf("first = %d, second = %d\n", mem->first, mem->second);

	return 0;
}

