#include <iostream>
#include <algorithm>
#include <set>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

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

std::string getSockName(int sockId)
{
	struct sockaddr_in addr;
	unsigned int size = sizeof(addr);
	getsockname(sockId, (struct sockaddr*)(&addr), &size);
	char ip[INET_ADDRSTRLEN];
	unsigned int port;
	inet_ntop(AF_INET, (&addr.sin_addr), ip, sizeof(ip));
	port = ntohs(addr.sin_port);
	std::string ipStr = ip;
	std::string res = std::to_string(sockId) + " socket " + ipStr + ":" + std::to_string(port);
	return res;
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
				std::string msg = "connected " + getSockName(slaveSocket) + "\n";
				for (auto s : slaveSockets)
				{
					std::string msgb = "connected " + getSockName(s) + "\n";
					send(s, msg.data(), msg.size(), MSG_NOSIGNAL);
					send(slaveSocket, msgb.data(), msgb.size(), MSG_NOSIGNAL);
				}
				slaveSockets.insert(slaveSocket);
			}
			else
			{
				static char buffer[1024];
				int recvSize = recv(events[i].data.fd,buffer,1024,MSG_NOSIGNAL);
				if (recvSize == 0 && errno != EAGAIN)
				{
					auto msg = "disconnected " + getSockName(events[i].data.fd) + "\n";
					slaveSockets.erase(events[i].data.fd);
					for (auto s : slaveSockets)
					{
						send(s, msg.data(), msg.size(), MSG_NOSIGNAL);
					}
					shutdown(events[i].data.fd, SHUT_RDWR);
					close(events[i].data.fd);
				}
				else if(recvSize > 0)
				{					
					for (auto s : slaveSockets)
					{
						if (s != events[i].data.fd)
							send(s, buffer, recvSize, MSG_NOSIGNAL);
					}
				}
			}
		}
	}
	return 0;
}
