#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/thread/mutex.hpp>

#include <memory>
#include <unordered_map>


class SocketIOSession;

class SocketIOSessionManager : public std::enable_shared_from_this<SocketIOSessionManager>,
	boost::basic_lockable_adapter<boost::mutex>
{
public:
	void CreateNewSession(boost::asio::ip::tcp::socket&& socket);

	void Broadcast();
	void Send(const boost::uuids::uuid& session_id);

private:
	std::unordered_map<boost::uuids::uuid, SocketIOSession> sessions_;
};

