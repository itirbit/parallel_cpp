#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	if(fork())
	{
		if(fork())
		{
			if(fork())
			{
				fork();
			}
		}
	}
	printf("Hello\n");
	
	//printf("pid = %d parentPid = %d\n",getpid(), getppid());
}
