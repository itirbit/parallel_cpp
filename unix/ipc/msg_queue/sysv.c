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
	printf("queue id = %d\n",queueId);
	struct message snd_msg;
	if(fork())
	{	
		snd_msg.mtype = 1;
		strcpy(snd_msg.mtext, "My message. IT'S ALIVE\0");
		int m = msgsnd(queueId, &snd_msg, sizeof(snd_msg.mtext), 0);
	}
	else	
	{
		struct message msg;
		int m = msgrcv(queueId, &msg, sizeof(msg.mtext), 1, 0);
		FILE* fd = fopen("/tmp/message.txt", "rw");
		fputs(msg.mtext, fd);
		fclose(fd);
		printf("message: %s\n", msg.mtext);
	}
	return 0;
}
