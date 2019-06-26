#pragma once

#include <boost/thread.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

class SocketIOThread : public boost::thread
{
public:
	SocketIOThread(std::shared_ptr<boost::beast::net::io_context> io_context);

	void Start();

private:
	std::shared_ptr<boost::beast::net::io_context> io_context_;
};
