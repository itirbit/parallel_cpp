#include <iostream>
#include <algorithm>
#include <set>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

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

	while(1)
	{
		fd_set set;
		FD_ZERO(&set);
		FD_SET(masterSocket, &set);
		for(auto it = slaveSockets.begin(); it != slaveSockets.end(); ++it)
		{
			FD_SET(*it, &set);
		}
		int max = std::max(masterSocket, *std::max_element(slaveSockets.begin(), slaveSockets.end()));
		select(max+1, &set, NULL, NULL, NULL);

		for (auto it = slaveSockets.begin(); it != slaveSockets.end(); )
		{
			if(FD_ISSET(*it, &set))
			{
				static char buffer[1024];
				int recvSize = recv(*it, buffer, 1024, MSG_NOSIGNAL);
				if (recvSize == 0 && (errno != EAGAIN))
				{
					shutdown(*it, SHUT_RDWR);
					it = slaveSockets.erase(it);
				}
				else if (recvSize != 0)
				{
					send(*it, buffer, recvSize, MSG_NOSIGNAL);
					++it;
				}
			}
			else 
				++it;	
		}
		if (FD_ISSET(masterSocket, &set))
		{
			int slaveSocket = accept(masterSocket, 0, 0);
			set_nonblock(slaveSocket);
			slaveSockets.insert(slaveSocket);
		}
	}

	return 0;
}
