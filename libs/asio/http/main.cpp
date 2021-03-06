#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"

int main(int argc, char** argv)
{
	try
	{
		if(argc != 4)
		{
			std::cerr << "Usage: http_server <address><port><doc_root>\n";
			std::cerr << " For ipv4, try:\n   receiver 0.0.0.0 80.\n";
			std::cerr << " For ipv6, try:\n   receiver 0::0 80.\n";
			return 1;
		}
		http::server::server s(argv[1],argv[2], argv[3]);
		s.run();
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

