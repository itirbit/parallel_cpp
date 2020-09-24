#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	int sv[2];
	int res = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
	if(fork())
	{
		char data[1024];
		int nbytes = read(sv[0], data, 1024);
		printf("%s",data);
	}
	else
	{
		char data[] = "some message\n";
		write(sv[1], data, sizeof(data));
	}
	close(sv[0]);
	close(sv[1]);
	return 0;
}
