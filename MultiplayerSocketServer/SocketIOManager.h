#pragma once
#include <vector>
#include <thread>
#include <boost/thread/thread_pool.hpp>
#include <boost/thread.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/beast/core.hpp>

#include "SocketIOListener.h"

class SocketIOManager
{
public:
	SocketIOManager(const int& thread_count);

	bool Initialize(std::shared_ptr<SocketIOListener> listener);

	void Start();
	void Stop();

	std::shared_ptr<boost::beast::net::io_context> GetIOContext()
	{
		return io_context_;
	}
protected:
	boost::thread_group threads_;
	std::shared_ptr<SocketIOListener> listener_;
	std::shared_ptr<boost::beast::net::io_context> io_context_;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> io_work_gaurd_;
};

