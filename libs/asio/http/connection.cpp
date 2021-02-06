#include "connection.hpp"
#include <utility>
#include <vector>
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

connection::connection(boost::asio::ip::tcp::socket socket,
	connection_manager& manager, request_handler& handler)
	: socket(std::move(socket)),
	connection_manager_(manager),
	mRequest_handler(handler)
{
}

void connection::start()
{
	do_read();
}

void connection::stop()
{
	socket.close();
}

void connection::do_read()
{
	auto self(shared_from_this());
	socket.async_read_some(boost::asio::buffer(buffer),
		[this, self](boost::system::error_code ec, std::size_t bytes_transferred)
		{
			if(!ec)
			{
				request_parser::result_type result;
				std::tie(result, std::ignore) = mRequest_parser.parse(mRequest,
					buffer.data(), buffer.data() + bytes_transferred);
				if (result == request_parser::good)
				{
					mRequest_handler.handle_request(mRequest, mReply);
					do_write();
				}
				else if (result == request_parser::bad)
				{
					mReply = reply::stock_reply(reply::bad_request);
					do_write();
				}
				else
				{
					do_read();
				}
			}
			else if(ec != boost::asio::error::operation_aborted)
			{
				connection_manager_.stop(shared_from_this());
			}
		});
}

void connection::do_write()
{
	auto self(shared_from_this());
	boost::asio::async_write(socket, mReply.to_buffers(),
		[this, self](boost::system::error_code ec, std::size_t)
		{
			if(!ec)
			{
				boost::system::error_code ignored_ec;
				socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
				ignored_ec);
			}
			if (ec != boost::asio::error::operation_aborted)
			{
				connection_manager_.stop(shared_from_this());
			}
		});
}


}
}







