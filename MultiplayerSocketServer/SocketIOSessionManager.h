#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/functional/hash.hpp>

#include <memory>
#include <unordered_map>


class SocketIOSession;

//template<>
//struct std::hash<boost::uuids::uuid>
//{
//	size_t operator () (const boost::uuids::uuid& uid)
//	{
//		return boost::hash<boost::uuids::uuid>()(uid);
//	}
//};

class SocketIOSessionManager : public std::enable_shared_from_this<SocketIOSessionManager>,
	public boost::basic_lockable_adapter<boost::mutex>
{
public:
	void CreateNewSession(boost::asio::ip::tcp::socket&& socket);

	void Broadcast();
	void Send(const boost::uuids::uuid& session_id);

private:
	std::unordered_map<boost::uuids::uuid, std::shared_ptr<SocketIOSession>, boost::hash<boost::uuids::uuid>> sessions_;
};

