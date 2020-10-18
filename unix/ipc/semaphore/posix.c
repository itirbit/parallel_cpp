#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	sem_unlink("/test.sem");
	sem_t* s = sem_open("/test.sem", O_CREAT | O_EXCL, 0666, 66);
	if (s == SEM_FAILED)
	{
		perror("failed sem_open");
		return 1;
	}
	//printf("OK");
	return 0;
}
