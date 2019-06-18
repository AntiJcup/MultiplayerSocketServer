#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

class SocketIOSessionManager;

class SocketIOSession : public std::enable_shared_from_this<SocketIOSession>
{
public:
	SocketIOSession(boost::asio::ip::tcp::socket&& socket, std::shared_ptr<SocketIOSessionManager> session_manager);

	void Run();

	boost::uuids::uuid GetId()
	{
		return id_;
	}

protected:
	void OnAccept(boost::beast::error_code ec);

	void DoRead();

	virtual void OnRead(boost::beast::error_code ec,
		std::size_t bytes_transferred);

	virtual void OnWrite(boost::beast::error_code ec,
		std::size_t bytes_transferred);

	virtual void OnData();

private:
	boost::beast::websocket::stream<boost::beast::tcp_stream> web_socket_stream_;
	boost::beast::flat_buffer buffer_;
	boost::uuids::uuid id_{ boost::uuids::random_generator()() }
	std::shared_ptr<SocketIOSessionManager> session_manager_;
};

