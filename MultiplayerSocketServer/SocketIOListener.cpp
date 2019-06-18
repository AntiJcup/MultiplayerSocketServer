#include "SocketIOListener.h"
#include "SocketIOException.h"
#include "SocketIOSession.h"

#include <boost/asio/strand.hpp>
#include <memory>

SocketIOListener::SocketIOListener(boost::beast::net::io_context& io_context,
	boost::asio::ip::tcp::endpoint end_point,
	std::shared_ptr<SocketIOSessionManager> session_manager)
	: io_context_(io_context), acceptor_(io_context), session_manager_(session_manager)
{
	boost::beast::error_code ec;

	// Open the acceptor
	acceptor_.open(end_point.protocol(), ec);
	if (ec)
	{
		throw SocketIOException("Failed opening acceptor", ec);
		return;
	}

	// Allow address reuse
	acceptor_.set_option(boost::beast::net::socket_base::reuse_address(true), ec);
	if (ec)
	{
		throw SocketIOException("Failed set_option acceptor", ec);
		return;
	}

	// Bind to the server address
	acceptor_.bind(end_point, ec);
	if (ec)
	{
		throw SocketIOException("Failed bind acceptor", ec);
		return;
	}

	// Start listening for connections
	acceptor_.listen(
		boost::beast::net::socket_base::max_listen_connections, ec);
	if (ec)
	{
		throw SocketIOException("Failed listen acceptor", ec);
		return;
	}
}

void SocketIOListener::Run()
{
	DoAccept();
}

void SocketIOListener::DoAccept()
{
	acceptor_.async_accept(
		boost::asio::make_strand(io_context_),
		boost::beast::bind_front_handler(
			&SocketIOListener::OnAccept,
			shared_from_this()));
}

void SocketIOListener::OnAccept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
{
	if (ec)
	{
		throw SocketIOException("OnAccept", ec);
	}
	else
	{
		session_manager_->CreateNewSession(std::move(socket));
	}

	// Accept another connection
	DoAccept();
}