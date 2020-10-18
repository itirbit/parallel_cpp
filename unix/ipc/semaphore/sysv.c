#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	key_t key = ftok("tmp/sem.temp", 1);
	int semId = semget(key, 16, IPC_CREAT | 0666);
	if (semId == -1)
	{
		perror("failed semget");
		return 1;
	}
	struct sembuf sbuf[1];
	for(int i=0; i<16; ++i)
	{
		sbuf[0].sem_num = i;
		sbuf[0].sem_op = i;
		semop(semId, sbuf, 1);
	}
	return 0;
}
