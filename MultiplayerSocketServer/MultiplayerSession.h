#pragma once

#include "SocketIOSession.h"

class MultiplayerSession
	: public SocketIOSession
{
public:
	MultiplayerSession(SocketIOSession* session);

	MultiplayerSession(boost::asio::ip::tcp::socket& socket, socket_io_session_manager_t session_manager);
};

typedef std::shared_ptr<MultiplayerSession> multiplayer_session_t;