#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char** argv)
{
	int masterSocket = socket(
		AF_INET, //IPv4
		SOCK_STREAM,//TCP
		IPPROTO_TCP
	);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);//0.0.0.0
	bind(masterSocket, (struct sockaddr*)(&addr),sizeof(addr));

	listen(masterSocket, SOMAXCONN);

	int slaveSocket = accept(masterSocket, 0, 0);
	while(1)
	{
		int buffer[1024];
		int size = recv(slaveSocket, buffer, 1024, MSG_NOSIGNAL);
		send(slaveSocket, buffer, size, MSG_NOSIGNAL);
		printf("%s\n", buffer);
	}
	shutdown(slaveSocket, SHUT_RDWR);
	close(slaveSocket);
	return 0;
}
