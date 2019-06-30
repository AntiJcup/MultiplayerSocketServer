#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <google/protobuf/message_lite.h>

typedef boost::uuids::uuid socket_io_session_id_t;
typedef boost::hash<socket_io_session_id_t> socket_io_session_id_hasher_t;

class SocketIOSession;
typedef std::shared_ptr<SocketIOSession> socket_io_session_t;

class SocketIOSessionManager;
typedef std::shared_ptr<SocketIOSessionManager> socket_io_session_manager_t;

typedef std::shared_ptr<google::protobuf::MessageLite> message_t;