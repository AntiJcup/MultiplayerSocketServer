#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/signals2/signal.hpp>

#include <google/protobuf/message.h>

class SocketIOSessionManager;

class SocketIOSession : public std::enable_shared_from_this<SocketIOSession>
{
public:
	typedef boost::signals2::signal<void(SocketIOSession*)> signal_t;

	SocketIOSession(boost::asio::ip::tcp::socket&& socket, std::shared_ptr<SocketIOSessionManager> session_manager);

	void Run();

	boost::uuids::uuid get_id()
	{
		return id_;
	}

	boost::signals2::connection ListenToDisconnect(const signal_t::slot_type& subscriber);

	void Send(const google::protobuf::Message &message);

protected:
	void OnAccept(boost::beast::error_code ec);

	void DoRead();

	virtual void OnRead(boost::beast::error_code ec,
		std::size_t bytes_transferred);

	void OnWrite(boost::beast::error_code ec,
		std::size_t bytes_transferred);

	void OnDisconnect();

private:
	boost::beast::websocket::stream<boost::beast::tcp_stream> web_socket_stream_;
	boost::beast::flat_buffer buffer_;
	boost::uuids::uuid id_{ boost::uuids::random_generator()() };
	std::shared_ptr<SocketIOSessionManager> session_manager_;
	signal_t disconnect_sig_;
};

