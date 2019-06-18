#include "SocketIOThread.h"

SocketIOThread::SocketIOThread(boost::beast::net::io_context& io_context)
	: io_context_(io_context)
{
	Start();
}

void SocketIOThread::Start()
{
	io_context_.run();
}