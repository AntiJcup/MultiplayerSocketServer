#include "SocketIOThread.h"

void SocketIOThread::Start()
{
	io_context_.run();
}