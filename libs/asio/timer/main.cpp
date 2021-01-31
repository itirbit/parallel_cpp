#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>

class printer
{
public:
	printer(boost::asio::io_context& io): 
		strand(boost::asio::make_strand(io)),
		timer1(io, boost::asio::chrono::seconds(1)),
		timer2(io, boost::asio::chrono::seconds(1)),
		count(0)
	{
		timer1.async_wait(boost::bind(&printer::print1, this));
		timer2.async_wait(boost::bind(&printer::print2, this));
	}
	~printer()
	{
		std::cout << "Final count is " << count << '\n';
	}

	void print1()
	{
		if (count < 10)
		{
			std::cout << "Timer 1: " << count << '\n';
			++count;
			timer1.expires_at(timer1.expiry() + boost::asio::chrono::seconds(1));
			timer1.async_wait(boost::asio::bind_executor(strand, boost::bind(&printer::print1, this)));
		}
	}
	void print2()
	{
		if (count < 10)
		{
			std::cout << "Timer 2: " << count << '\n';
			++count;
			timer2.expires_at(timer2.expiry() + boost::asio::chrono::seconds(1));
			timer2.async_wait(boost::asio::bind_executor(strand, boost::bind(&printer::print2, this)));
		}
	}
private:
	boost::asio::strand<boost::asio::io_context::executor_type> strand;
	boost::asio::steady_timer timer1;
	boost::asio::steady_timer timer2;
	int count;
};


int main()
{
	boost::asio::io_context io;
	printer p(io);
	boost::thread t(boost::bind(&boost::asio::io_context::run, &io));
	io.run();
	t.join();
	return 0;
}
