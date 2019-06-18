#pragma once
#include <vector>
#include <thread>
#include <boost/thread/thread_pool.hpp>
#include <boost/thread.hpp>
#include <boost/uuid/uuid.hpp>

class SocketIOManager
{
public:
	SocketIOManager();

	bool Initialize();

	void Start();
	void Stop();


protected:
	boost::thread_group threads_;
};

