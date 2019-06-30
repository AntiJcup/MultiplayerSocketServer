#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/signals2/signal.hpp>

#include <memory>
#include <unordered_map>

#include "WrapperMessage.pb.h"
#include "SocketIOCommonTypes.h"
#include "SocketIOSession.h"

class SocketIOSession;

class SocketIOSessionWrapper
{

public:
	boost::signals2::scoped_connection disconnect_connection;
	socket_io_session_t session;
};

class SocketIOSessionManager : public std::enable_shared_from_this<SocketIOSessionManager>,
	public boost::basic_lockable_adapter<boost::mutex>
{
	typedef std::unordered_map<socket_io_session_id_t, SocketIOSessionWrapper, socket_io_session_id_hasher_t> session_map_t;
public:
	virtual std::shared_ptr<SocketIOSession> CreateNewSession(boost::asio::ip::tcp::socket&& socket);
	void RemoveSession(const socket_io_session_id_t& session_id);

	void Broadcast(message_t message);
	void Send(const socket_io_session_id_t& session_id, message_t message);

protected:
	session_map_t sessions_;
};