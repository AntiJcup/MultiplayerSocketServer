#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/asio/streambuf.hpp>

#include <google/protobuf/message.h>

#include "SocketIOMessageParser.h"
#include "SocketIOCommonTypes.h"
#include "SocketIOSessionManager.h"

enum class SessionState
{
	Default,
	Connected,
	Disconnected,
};

class SocketIOSession : public std::enable_shared_from_this<SocketIOSession>
{
public:
	typedef boost::signals2::signal<void(socket_io_session_t)> disconnect_signal_t;
	typedef boost::signals2::signal<void(socket_io_session_t, message_t)> message_signal_t;

	SocketIOSession(boost::asio::ip::tcp::socket&& socket, socket_io_session_manager_t session_manager);

	virtual ~SocketIOSession() = default;
	SocketIOSession(SocketIOSession&&) = default;

	void Run();

	const socket_io_session_id_t& get_id() const
	{
		return id_;
	}

	SessionState get_state()
	{
		return state_;
	}

	boost::signals2::connection ListenToDisconnect(const disconnect_signal_t::slot_type& subscriber);
	boost::signals2::connection ListenToMessages(const message_signal_t::slot_type& subscriber);

	void Send(message_t message);

protected:
	void OnAccept(boost::beast::error_code ec);

	void DoRead();

	void OnRead(boost::beast::error_code ec,
		std::size_t bytes_transferred);

	void OnWrite(boost::beast::error_code ec,
		std::size_t bytes_transferred);

	void OnDisconnect();

	void OnMessage(message_t message);

	void Write(const boost::asio::streambuf& stream_buffer);

	void Write(const char* buffer, std::size_t buffer_size);

private:
	boost::beast::websocket::stream<boost::beast::tcp_stream> web_socket_stream_;
	boost::beast::flat_buffer buffer_;
	socket_io_session_id_t id_{ boost::uuids::random_generator()() };

	std::shared_ptr<SocketIOSessionManager> session_manager_;

	SocketIOMessageParser parser_;

	disconnect_signal_t disconnect_sig_;
	message_signal_t message_sig_;

	SessionState state_{ SessionState::Default };
};