#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void saveToFile(const char* path, char* data)
{
	FILE * f = fopen(path, "w");
	fputs(data, f);
	fclose(f);
}

int main(int argc, char** argv)
{
	char data[1024];	

	if (fork())
	{
		sprintf(data, "pid = %d, ppid = %d", getpid(), getppid());	
		saveToFile("/tmp/parent_stepik", data);
		signal(SIGABRT, SIG_IGN);
		signal(SIGTERM, SIG_IGN);
		signal(SIGCHLD, SIG_IGN);
	}
	else
	{
		sprintf(data, "pid = %d, ppid = %d", getpid(), getppid());	
		saveToFile("/tmp/child_stepik", data);
		signal(SIGABRT, SIG_IGN);
		signal(SIGTERM, SIG_DFL);
	}

	while(1)
		sleep(1);
	

	return 0;
}
