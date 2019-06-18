#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

class SocketIOSession
{
public:
	SocketIOSession(boost::asio::ip::tcp::socket&& socket);

	void Run();

	void OnAccept(boost::beast::error_code ec);

	void DoRead(boost::beast::error_code ec,
		std::size_t bytes_transferred);

	void OnWrite(boost::beast::error_code ec,
		std::size_t bytes_transferred);
};

