#pragma once

#include "SocketIOSession.h"

class MultiplayerSession : public SocketIOSession
{
public:
	MultiplayerSession(SocketIOSession* session);
};

