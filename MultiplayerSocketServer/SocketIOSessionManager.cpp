#include "SocketIOSessionManager.h"
#include "SocketIOSession.h"

#include <boost/thread/lock_guard.hpp>

void SocketIOSessionManager::CreateNewSession(boost::asio::ip::tcp::socket&& socket)
{
	boost::lock_guard<SocketIOSessionManager> guard(*this);
	auto session = std::make_shared<SocketIOSession>(std::move(socket), shared_from_this());
	sessions_[session->GetId()] = session;
	session->Run();
}

void SocketIOSessionManager::Broadcast()
{
	boost::lock_guard<SocketIOSessionManager> guard(*this);
}

void SocketIOSessionManager::Send(const boost::uuids::uuid &session_id)
{
	boost::lock_guard<SocketIOSessionManager> guard(*this);
	sessions_[session_id]->
}
