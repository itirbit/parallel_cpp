#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>

#define PERM 0644
#define QFN "/test-queue"

int main(int argc, char** argv)
{
	//mq_unlink(fn);
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 16;
	attr.mq_msgsize = 80;
	attr.mq_curmsgs = 0;

	mqd_t mqd = mq_open(QFN, O_CREAT | O_WRONLY, PERM, &attr);
	if (mqd == -1)
	{
		perror("failed to mq_open");
	}
	char msg[80];
	strcpy(msg, "HELLO WORLD! from POSIX");
	while(1)
	{
		sleep(1);
		int res = mq_send(mqd, msg, strlen(msg)+1, 0);
		if (res == -1)
		{	
			perror("failed to mq_send");
		}
		printf("send message:%s\n",msg);
	}	
	//mq_close(mqd);
	//mq_unlink(fn);
	return 0;
}
