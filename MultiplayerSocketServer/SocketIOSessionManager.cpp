#include "SocketIOSessionManager.h"
#include "SocketIOSession.h"

#include <boost/thread/lock_guard.hpp>

std::shared_ptr<SocketIOSession> SocketIOSessionManager::CreateNewSession(boost::asio::ip::tcp::socket&& socket)
{
	boost::lock_guard<SocketIOSessionManager> guard(*this);
	auto session = std::make_shared<SocketIOSession>(std::move(socket), shared_from_this());
	sessions_[session->get_id()] = session;
	session->Run();

	return session;
}

void SocketIOSessionManager::Broadcast(std::shared_ptr<google::protobuf::MessageLite> message)
{
	boost::lock_guard<SocketIOSessionManager> guard(*this);

	for (auto& session : sessions_)
	{
		session.second->Send(*message);
	}
}

void SocketIOSessionManager::Send(const boost::uuids::uuid& session_id, std::shared_ptr<google::protobuf::MessageLite> message)
{
	boost::lock_guard<SocketIOSessionManager> guard(*this);
	sessions_[session_id]->Send(*message);
	/*sessions_[session_id]->*/
}
