#include "SocketIOSession.h"
#include "SocketIOException.h"
#include "SocketIOSessionManager.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

SocketIOSession::SocketIOSession(boost::asio::ip::tcp::socket&& socket, std::shared_ptr<SocketIOSessionManager> session_manager)
	: web_socket_stream_(std::move(socket)), session_manager_(session_manager)
{
}

void SocketIOSession::Run()
{
	// Set suggested timeout settings for the websocket
	web_socket_stream_.set_option(
		boost::beast::websocket::stream_base::timeout::suggested(
			boost::beast::role_type::server));

	// Set a decorator to change the Server of the handshake
	web_socket_stream_.set_option(boost::beast::websocket::stream_base::decorator(
		[](boost::beast::websocket::response_type& res)
		{
			res.set(boost::beast::http::field::server,
				std::string(BOOST_BEAST_VERSION_STRING) +
				" websocket-server-async");
		}));

	// Accept the websocket handshake
	web_socket_stream_.async_accept(
		boost::beast::bind_front_handler(
			&SocketIOSession::OnAccept,
			shared_from_this()));
}

boost::signals2::connection SocketIOSession::ListenToDisconnect(const disconnect_signal_t::slot_type& subscriber)
{
	return disconnect_sig_.connect(subscriber);
}

boost::signals2::connection SocketIOSession::ListenToMessages(const message_signal_t::slot_type& subscriber)
{
	return message_sig_.connect(subscriber);
}

void SocketIOSession::Send(const google::protobuf::MessageLite& message)
{
	boost::asio::streambuf stream_buffer;
	std::ostream output_stream(&stream_buffer);
	{
		google::protobuf::io::OstreamOutputStream raw_output_stream(&output_stream);
		google::protobuf::io::CodedOutputStream coded_output_stream(&raw_output_stream);
		/*coded_output_stream.WriteVarint32(message.ByteSize());*/

		message.SerializeToCodedStream(&coded_output_stream);
		// IMPORTANT: In order to flush a CodedOutputStream it has to be deleted,
		// otherwise a 0 bytes package is send over the wire.
	}

	Write(stream_buffer);
}

void SocketIOSession::Write(const boost::asio::streambuf& stream_buffer)
{
	web_socket_stream_.write(stream_buffer.data());
}


void SocketIOSession::Write(const char* buffer, std::size_t buffer_size)
{
	web_socket_stream_.write(boost::asio::buffer(buffer, buffer_size));
}

void SocketIOSession::OnAccept(boost::beast::error_code ec)
{
	if (ec)
		throw SocketIOException("failed accept", ec);

	state_ = SessionState::Connected;

	// Read a message
	DoRead();
}

void SocketIOSession::DoRead()
{
	web_socket_stream_.async_read(
		buffer_,
		boost::beast::bind_front_handler(
			&SocketIOSession::OnRead,
			shared_from_this()));
}

void SocketIOSession::OnRead(
	boost::beast::error_code ec,
	std::size_t bytes_transferred)
{
	// This indicates that the session was closed
	if (ec == boost::beast::websocket::error::closed) {
		OnDisconnect();
		return;
	}

	if (ec)
		throw SocketIOException("failed read", ec);

	if (auto message_ptr = parser_.TryParseMessage(bytes_transferred, buffer_))
	{
		OnMessage(message_ptr);
	}

	DoRead();
}

void SocketIOSession::OnWrite(boost::beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if (ec) {
		throw SocketIOException("failed write", ec);
	}

	// Clear the buffer
	buffer_.consume(buffer_.size());

	// Do another read
	DoRead();
}

void SocketIOSession::OnDisconnect()
{
	state_ = SessionState::Disconnected;
	disconnect_sig_(this); //Fire disconnect event
}

void SocketIOSession::OnMessage(std::shared_ptr<google::protobuf::MessageLite> message)
{
	message_sig_(this, message);
}
