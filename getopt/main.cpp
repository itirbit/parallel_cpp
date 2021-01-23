#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int opt;
	while( (opt = getopt(argc,argv, "hi:p:")) != -1)
	{
		switch(opt)
		{
		case 'h': printf("show help information\n"); break;
		case 'i': printf("found argument i=%s\n", optarg); break;
		case 'p': printf("found argument p=%s\n", optarg); break;
		}
	}
	return 0;
}	
