#include <iostream>
#include <algorithm>
#include <set>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define POLL_SIZE 2048

int set_nonblock(int fd)
{
	int flags;	
#if defined(O_NONBLOCK)
	if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	flags = 1;
	return ioctl(fd, FIONBIO, &flags);
#endif
}

int main (int argc, char** argv)
{
	int masterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	std::set<int> slaveSockets;

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(masterSocket, (struct sockaddr*)(&addr), sizeof(addr));

	set_nonblock(masterSocket);
	
	listen(masterSocket, SOMAXCONN);

	struct pollfd set[POLL_SIZE];
	set[0].fd = masterSocket;
	set[0].events = POLLIN;

	while(1)
	{
		unsigned int index = 1;
		for (auto it = slaveSockets.begin(); it != slaveSockets.end(); ++it)
		{
			set[index].fd = *it;
			set[index].events = POLLIN;
			++index;
		}

		unsigned int setSize = 1 + slaveSockets.size();

		poll(set, setSize, -1);
		
		for (unsigned int i = 0; i < setSize; ++i)
		{
			if(set[i].revents & POLLIN)
			{
				if(i)
				{
					static char buffer[1024];
					int recvSize = recv(set[i].fd, buffer, 1024, MSG_NOSIGNAL);
					if ((recvSize == 0) && (errno != EAGAIN))
					{
						shutdown(set[i].fd, SHUT_RDWR);
						close(set[i].fd);
						slaveSockets.erase(set[i].fd);
					}
					else if (recvSize > 0)
					{
						send(set[i].fd, buffer, recvSize, MSG_NOSIGNAL);
					}
				}
				else
				{
					int slaveSocket = accept(masterSocket,0,0);
					set_nonblock(slaveSocket);
					slaveSockets.insert(slaveSocket);
				}
			}
		}
	}
	return 0;
}
