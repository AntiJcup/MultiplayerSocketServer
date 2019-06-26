#include "SocketIOThread.h"

SocketIOThread::SocketIOThread(std::shared_ptr<boost::beast::net::io_context> io_context)
	: io_context_(io_context), boost::thread(&SocketIOThread::Start, this)
{
	
}

void SocketIOThread::Start()
{
	io_context_->run();
}