#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	char data[1024];
	sprintf(data, "%d", getpid());
	printf("%s", data);

	FILE * f = fopen("/tmp/pid_stepik", "w");
	fputs(data, f);
	fclose(f);

	signal(SIGKILL, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	while(1)
		sleep(1);
	

	return 0;
}
