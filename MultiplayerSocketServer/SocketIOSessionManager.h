#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/functional/hash.hpp>

#include <memory>
#include <unordered_map>

#include "WrapperMessage.pb.h"

class SocketIOSession;

class SocketIOSessionManager : public std::enable_shared_from_this<SocketIOSessionManager>,
	public boost::basic_lockable_adapter<boost::mutex>
{
public:
	void CreateNewSession(boost::asio::ip::tcp::socket&& socket);

	void Broadcast(std::shared_ptr<google::protobuf::MessageLite> message);
	void Send(const boost::uuids::uuid& session_id, std::shared_ptr<google::protobuf::MessageLite> message);

private:
	std::unordered_map<boost::uuids::uuid, std::shared_ptr<SocketIOSession>, boost::hash<boost::uuids::uuid>> sessions_;
};

