#include <iostream>
#include <boost/asio.hpp>

void print(const boost::system::error_code&)
{
	std::cout << "Hello, world from print!\n";
}

int main()
{
	boost::asio::io_context io;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
	t.async_wait(&print);
	io.run();
	std::cout << "Hello world from main!\n";
	return 0;
}
