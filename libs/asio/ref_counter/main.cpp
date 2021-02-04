#include <boost/asio.hpp>
#include <iostream>
#include <utility>
#include <vector>
#include <ctime>
#include <memory>

using boost::asio::ip::tcp;

class shared_const_buffer
{
public:
	explicit shared_const_buffer(const std::string& msg) :
		data(new std::vector<char>(msg.begin(), msg.end())),
		buffer(boost::asio::buffer(*data))
	{
	}

	typedef boost::asio::const_buffer value_type;
	typedef const boost::asio::const_buffer* const_iterator;
	const boost::asio::const_buffer* begin() const { return &buffer;}
	const boost::asio::const_buffer* end() const { return &buffer + 1;}
private:
	std::shared_ptr<std::vector<char>> data;
	boost::asio::const_buffer buffer;
};

class session : public std::enable_shared_from_this<session>
{
public:
	session(tcp::socket socket) : socket(std::move(socket)) {}
	void start() { do_write();}
private:
	void do_write()
	{
		std::time_t now = std::time(0);
		shared_const_buffer buffer(std::ctime(&now));
		auto self(shared_from_this());
		boost::asio::async_write(socket, buffer, [self](boost::system::error_code, std::size_t){});
	}

	tcp::socket socket;
};

class server
{
public:
	server(boost::asio::io_context& io, short port) :
		acceptor(io, tcp::endpoint(tcp::v4(), port))
	{
		do_accept();
	}
private:
	void do_accept()
	{
		acceptor.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket)
			{
				if(!ec)
				{
					std::make_shared<session>(std::move(socket))->start();
				}
				do_accept();
			});
	}

	tcp::acceptor acceptor;
};

int main(int argc, char** argv)
{
	try
	{
		if(argc != 2)
		{
			std::cerr << "Usage: reference_counted <port>\n";
			return 1;
		}
		boost::asio::io_context io;
		server s(io, std::atoi(argv[1]));
		io.run();
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	return 0;
}
