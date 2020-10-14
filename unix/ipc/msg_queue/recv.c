#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

struct message {
	long mtype;
	char mtext[80];
};


int main(int argc, char** argv)
{
	key_t key = ftok("/tmp/msg.temp", 1);
	int queueId = msgget(key, IPC_CREAT | 0660);
	struct message msg;
	while(1)
	{
		int m = msgrcv(queueId, &msg, sizeof(msg.mtext), 1, 0);
		FILE* fd = fopen("/tmp/message.txt", "rw");
		fputs(msg.mtext, fd);
		fclose(fd);
		printf("message: %s\n", msg.mtext);
	}
	return 0;
}
