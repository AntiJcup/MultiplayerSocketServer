#include "SocketIOSessionManager.h"
#include "SocketIOSession.h"

#include <boost/thread/lock_guard.hpp>

std::shared_ptr<SocketIOSession> SocketIOSessionManager::CreateNewSession(boost::asio::ip::tcp::socket&& socket)
{
	auto session = std::make_shared<SocketIOSession>(std::move(socket), shared_from_this());
	session->Run();

	{
		boost::lock_guard<SocketIOSessionManager> guard(*this);

		SocketIOSessionWrapper session_wrapper = {};
		session_wrapper.session = session;
		session_wrapper.disconnect_connection = session->ListenToDisconnect(
			[this](auto session) {
				RemoveSession(session->get_id());
			});
		sessions_[session->get_id()] = std::move(session_wrapper);
	}

	return session;
}

void SocketIOSessionManager::RemoveSession(const socket_io_session_id_t &session_id)
{
	boost::lock_guard<SocketIOSessionManager> guard(*this);
	sessions_.erase(session_id);
}

void SocketIOSessionManager::Broadcast(message_t message)
{
	boost::lock_guard<SocketIOSessionManager> guard(*this);

	for (auto& session : sessions_)
	{
		session.second.session->Send(message);
	}
}

void SocketIOSessionManager::Send(const socket_io_session_id_t& session_id, message_t message)
{
	boost::lock_guard<SocketIOSessionManager> guard(*this);
	sessions_[session_id].session->Send(message);
}
