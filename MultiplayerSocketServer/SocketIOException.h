#pragma once

#include <exception>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

class SocketIOException : public std::exception
{
public:
	SocketIOException(const char* msg, boost::beast::error_code ec)
		: error_code(ec), std::exception(msg)
	{

	}


	boost::beast::error_code error_code;
};

