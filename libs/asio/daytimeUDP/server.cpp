#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string()
{
	using namespace std;
	time_t now = time(0);
	return ctime(&now);
}

class udp_server
{
public:
	udp_server(boost::asio::io_context& io) :
		socket(io, udp::endpoint(udp::v4(),13))
	{
		start_receive();
	}
private:
	void start_receive()
	{
		socket.async_receive_from(boost::asio::buffer(recv_buffer), remote_endpoint,
				boost::bind(&udp_server::handle_receive, this, 
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	void handle_receive(const boost::system::error_code& error, std::size_t)
	{
		if(!error)
		{
			boost::shared_ptr<std::string> msg(new std::string(make_daytime_string()));
			socket.async_send_to(boost::asio::buffer(*msg), remote_endpoint, 
					boost::bind(&udp_server::handle_send, this, msg,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			start_receive();
		}
	}
	void handle_send(boost::shared_ptr<std::string>, const boost::system::error_code&, std::size_t){}

	udp::socket socket;
	udp::endpoint remote_endpoint;
	boost::array<char, 1> recv_buffer;
};

int main()
{
	try
	{
		boost::asio::io_context io;
		udp_server server(io);
		io.run();
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
