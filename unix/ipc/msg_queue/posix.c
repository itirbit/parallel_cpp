#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>

#define PERM 0660

int main(int argc, char** argv)
{
	const char* fn = "/testmq";
	//mq_unlink(fn);
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 256;
	attr.mq_curmsgs = 0;

	mqd_t mqd = mq_open(fn, O_CREAT | O_RDWR, PERM, &attr);
	if (mqd == -1)
	{
		perror("failed to mq_open");
	}
	char msg[80];
	if(fork())
	{
		strcpy(msg, "HELLO WORLD! from POSIX");
		int res = mq_send(mqd, msg, sizeof(msg), 0);
		if (res == -1)
		{
			perror("failed to mq_send");
		}	
		//mq_close(mqd);
		//mq_unlink(fn);
	}
	else
	{
		char rmsg[80];
		mq_receive(mqd, rmsg, sizeof(rmsg),0);
		printf("%s\n",rmsg);
		//mq_close(mqd);
		//mq_unlink(fn);
	}
	return 0;
}
