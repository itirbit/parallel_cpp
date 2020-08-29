#include <iostream>
#include <algorithm>
#include <set>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>


#define MAX_EVENTS 32 

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

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(masterSocket, (struct sockaddr*)(&addr), sizeof(addr));

	set_nonblock(masterSocket);
	
	listen(masterSocket, SOMAXCONN);

	int ePoll = epoll_create1(0);
	
	struct epoll_event event;
	event.data.fd = masterSocket;
	event.events = EPOLLIN;
	epoll_ctl(ePoll, EPOLL_CTL_ADD, masterSocket, &event);

	while(true)
	{
		struct epoll_event events[MAX_EVENTS];
		int n = epoll_wait(ePoll, events, MAX_EVENTS, -1);
		for (unsigned int i = 0; i < n; ++i)
		{
			if (events[i].data.fd == masterSocket)
			{
				int slaveSocket = accept(masterSocket,0,0);
				set_nonblock(slaveSocket);
				struct epoll_event event;
				event.data.fd = slaveSocket;
				event.events = EPOLLIN;
				epoll_ctl(ePoll, EPOLL_CTL_ADD, slaveSocket, &event);
			}
			else
			{
				static char buffer[1024];
				int recvSize = recv(events[i].data.fd,buffer,1024,MSG_NOSIGNAL);
				if (recvSize == 0 && errno != EAGAIN)
				{
					shutdown(events[i].data.fd, SHUT_RDWR);
					close(events[i].data.fd);
				}
				else if(recvSize > 0)
				{
					send(events[i].data.fd, buffer, recvSize, MSG_NOSIGNAL);
				}
			}
		}
	}
	return 0;
}
