#include "SocketIOManager.h"
#include "SocketIOThread.h"

SocketIOManager::SocketIOManager(const int& thread_count)
	: io_context_(new boost::beast::net::io_context(thread_count)), io_work_gaurd_(boost::asio::make_work_guard(*io_context_))
{
	for (auto i = 0; i < thread_count; ++i) {
		threads_.add_thread(new SocketIOThread(io_context_));
	}
}

bool SocketIOManager::Initialize(std::shared_ptr<SocketIOListener> listener)
{
	listener_ = listener;
	return true;
}

void SocketIOManager::Start()
{
	listener_->Run();
	threads_.join_all();
}

void SocketIOManager::Stop()
{
	io_work_gaurd_.reset();
	io_context_->stop();
}