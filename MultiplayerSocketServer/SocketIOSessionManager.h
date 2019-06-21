#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/functional/hash.hpp>
#include <boost/signals2/signal.hpp>

#include <memory>
#include <unordered_map>

#include "WrapperMessage.pb.h"

class SocketIOSession;

class SocketIOSessionWrapper
{
public:
	boost::signals2::scoped_connection disconnect_connection;
	std::shared_ptr<SocketIOSession> session;
};

class SocketIOSessionManager : public std::enable_shared_from_this<SocketIOSessionManager>,
	public boost::basic_lockable_adapter<boost::mutex>
{
public:
	virtual std::shared_ptr<SocketIOSession> CreateNewSession(boost::asio::ip::tcp::socket&& socket);
	void RemoveSession(boost::uuids::uuid session_id);

	void Broadcast(std::shared_ptr<google::protobuf::MessageLite> message);
	void Send(const boost::uuids::uuid& session_id, std::shared_ptr<google::protobuf::MessageLite> message);

protected:
	std::unordered_map<boost::uuids::uuid, SocketIOSessionWrapper, boost::hash<boost::uuids::uuid>> sessions_;
};

