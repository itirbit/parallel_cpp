#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv)
{
	int fd = shm_open("/test.shm", O_CREAT | O_RDWR, 0666);
	if (fd == -1)
	{
		perror("failed shm_open");	
		return 1;
	}
	int res = ftruncate(fd, 1024*1024);
	if (res == -1)
	{
		perror("failed ftruncate");
		return 1;
	}
	char* mem = mmap(NULL, 1024*1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(*(int*)mem == -1)
	{
		perror("failed mmap");
		return 1;
	}
	memset(mem,(char)13,1024*1024);

	//for(int i=0; i<1024*1024; ++i)
	//{
	//	printf("%d",((char*)mem)[i]);
	//}

	return 0;
}
