#pragma once

#include "SocketIOSession.h"

class MultiplayerSession : public SocketIOSession
{
public:
	MultiplayerSession(SocketIOSession* session);

	MultiplayerSession(boost::asio::ip::tcp::socket&& socket, std::shared_ptr<SocketIOSessionManager> session_manager);
};

