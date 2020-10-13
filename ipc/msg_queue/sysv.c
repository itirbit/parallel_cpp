#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
		int m = msgsnd(queueId, &snd_msg, 80, 0);
	}
	else	
	{
		sleep(1);
		struct message msg;
		int m = msgrcv(queueId, &msg, 80, 1, 0);
		printf("message: %s\n", msg.mtext);
	}
	return 0;
}
