#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/asio/streambuf.hpp>

#include <google/protobuf/message.h>

#include "SocketIOMessageParser.h"

class SocketIOSessionManager;

enum class SessionState
{
	Default,
	Connected,
	Disconnected,
};

class SocketIOSession : public std::enable_shared_from_this<SocketIOSession>
{
public:
	typedef boost::signals2::signal<void(std::shared_ptr<SocketIOSession>)> disconnect_signal_t;
	typedef boost::signals2::signal<void(std::shared_ptr<SocketIOSession>, std::shared_ptr<google::protobuf::MessageLite>)> message_signal_t;

	SocketIOSession(boost::asio::ip::tcp::socket&& socket, std::shared_ptr<SocketIOSessionManager> session_manager);

	virtual ~SocketIOSession() = default;
	SocketIOSession(SocketIOSession&&) = default;

	void Run();

	boost::uuids::uuid get_id()
	{
		return id_;
	}

	SessionState get_state()
	{
		return state_;
	}

	boost::signals2::connection ListenToDisconnect(const disconnect_signal_t::slot_type& subscriber);
	boost::signals2::connection ListenToMessages(const message_signal_t::slot_type& subscriber);

	void Send(const google::protobuf::MessageLite& message);

protected:
	void OnAccept(boost::beast::error_code ec);

	void DoRead();

	void OnRead(boost::beast::error_code ec,
		std::size_t bytes_transferred);

	void OnWrite(boost::beast::error_code ec,
		std::size_t bytes_transferred);

	void OnDisconnect();

	void OnMessage(std::shared_ptr<google::protobuf::MessageLite> message);

	void Write(const boost::asio::streambuf& stream_buffer);

	void Write(const char* buffer, std::size_t buffer_size);

private:
	boost::beast::websocket::stream<boost::beast::tcp_stream> web_socket_stream_;
	boost::beast::flat_buffer buffer_;
	boost::uuids::uuid id_{ boost::uuids::random_generator()() };

	std::shared_ptr<SocketIOSessionManager> session_manager_;

	SocketIOMessageParser parser_;

	disconnect_signal_t disconnect_sig_;
	message_signal_t message_sig_;

	SessionState state_{ SessionState::Default };
};

