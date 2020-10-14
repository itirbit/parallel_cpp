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
	mqd_t mqd = mq_open(fn, O_RDONLY);
	if (mqd == -1)
	{
		perror("failed to mq_open");
	}
	char buff[80];
	while(1)
	{
		sleep(10);
		mq_receive(mqd, buff, sizeof(buff),0);
		printf("message: %s\n",buff);
	}
	//mq_close(mqd);
	//mq_unlink(fn);
	return 0;
}
