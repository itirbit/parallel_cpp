#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <error.h>
#include <string.h>

int main(int argc, char** argv)
{
	key_t key = ftok("/tmp/mem.tmp", 1);
	int shmId = shmget(key, (1024*1024), IPC_CREAT | 0666);
	if (shmId == -1)
	{
		perror("failed shmget");
		return 1;
	}
	char* mem = shmat(shmId, NULL, 0);
	if (*(int*)mem == -1)
	{
		perror("failed shmat");
		return 1;
	}
	memset(mem, (char)42, 1024*1024);
	
//	for (int i = 0; i < 1024*1024; ++i)
//	{
//		printf("%d", ((char*) mem)[i]);
//	}
	return 0;
}

