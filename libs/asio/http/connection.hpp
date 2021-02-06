#pragma once

#include <array>
#include <memory>
#include <boost/asio.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

namespace http {
namespace server {

class connection_manager;

class connection : public std::enable_shared_from_this<connection>
{
public:
	connection(const connection&) = delete;
	connection& operator=(const connection&) = delete;

	explicit connection(boost::asio::ip::tcp::socket socket,
			connection_manager& manager, request_handler& handler);
	void start();
	void stop();
private:
	void do_read();
	void do_write();
	boost::asio::ip::tcp::socket socket;
	connection_manager& connection_manager_;
	request_handler& mRequest_handler;
	std::array<char, 8192> buffer;
	request mRequest;
	request_parser mRequest_parser;
	reply mReply;
};

typedef std::shared_ptr<connection> connection_ptr;

}
}

