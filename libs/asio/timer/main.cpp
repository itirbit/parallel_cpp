#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>


void print(const boost::system::error_code&, boost::asio::steady_timer* t, int* count)
{
	std::cout << "Hello, world from print!\n";
	if (*count < 5)
	{
		std::cout << * count << '\n';
		++(*count);
		t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));
		t->async_wait(boost::bind(print, boost::asio::placeholders::error,t, count));
	}
}

int main()
{
	boost::asio::io_context io;
	int count;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
	t.async_wait(boost::bind(print, boost::asio::placeholders::error, &t, &count));
	io.run();
	std::cout << "Hello world from main!\n";
	std::cout << "Final Count is " << count << '\n';
	return 0;
}
