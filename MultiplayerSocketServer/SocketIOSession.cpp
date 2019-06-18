#include "SocketIOSession.h"
#include "SocketIOException.h"
#include "SocketIOSessionManager.h"

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

void SocketIOSession::OnAccept(boost::beast::error_code ec)
{
	if (ec)
		throw SocketIOException("failed accept", ec);

	// Read a message
	DoRead();
}

void SocketIOSession::DoRead()
{
}

void SocketIOSession::OnRead(
	boost::beast::error_code ec,
	std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	// This indicates that the session was closed
	if (ec == boost::beast::websocket::error::closed)
		return;

	if (ec)
		throw SocketIOException("failed read", ec);

	// Echo the message
	//web_socket_stream_.text(web_socket_stream_.got_text());
	/*web_socket_stream_.async_write(
		buffer_.data(),
		boost::beast::bind_front_handler(
			&SocketIOSession::OnWrite,
			shared_from_this()));*/
}

void SocketIOSession::OnWrite(boost::beast::error_code ec, std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if (ec)
		throw SocketIOException("failed write", ec);

	// Clear the buffer
	buffer_.consume(buffer_.size());

	// Do another read
	DoRead();
}
