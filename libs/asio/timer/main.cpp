#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

class printer
{
public:
	printer(boost::asio::io_context& io): 
		timer(io, boost::asio::chrono::seconds(1)),
		count(0)
	{
		timer.async_wait(boost::bind(&printer::print, this));
	}
	~printer()
	{
		std::cout << "Final count is " << count << '\n';
	}

	void print()
	{
		if (count < 5)
		{
			std::cout << count << '\n';
			++count;
			timer.expires_at(timer.expiry() + boost::asio::chrono::seconds(1));
			timer.async_wait(boost::bind(&printer::print, this));
		}
	}
private:
	boost::asio::steady_timer timer;
	int count;
};


int main()
{
	boost::asio::io_context io;
	printer p(io);
	io.run();
	return 0;
}
