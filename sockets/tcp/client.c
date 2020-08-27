#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main (int argc, char** argv)
{
	int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	connect(s, (struct sockaddr*)(&addr), sizeof(addr));
	
	char buffer[] = "PING";
	send(s, buffer, 4, MSG_NOSIGNAL);
	recv(s, buffer, 4, MSG_NOSIGNAL);
	
	shutdown(s, SHUT_RDWR);
	close(s);
	printf("%s\n", buffer);

	return 0;
}
