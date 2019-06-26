#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "SocketIOSessionManager.h"

class SocketIOListener : public std::enable_shared_from_this<SocketIOListener>
{
public:
	SocketIOListener(std::shared_ptr<boost::beast::net::io_context> io_context,
		boost::asio::ip::tcp::endpoint end_point,
		std::shared_ptr<SocketIOSessionManager> session_manager);

	void Run();

	std::shared_ptr<SocketIOSessionManager> GetSessionManager()
	{
		return session_manager_;
	}

private:
	std::shared_ptr<boost::beast::net::io_context> io_context_;
	boost::asio::ip::tcp::acceptor acceptor_;
	std::shared_ptr<SocketIOSessionManager> session_manager_;

	void DoAccept();

	void OnAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);
};

