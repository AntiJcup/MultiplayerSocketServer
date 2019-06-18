#pragma once

#include <boost/thread.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

class SocketIOThread : public boost::thread
{
public:
	SocketIOThread(boost::beast::net::io_context& io_context);

	void Start();

private:
	boost::beast::net::io_context& io_context_;
};
