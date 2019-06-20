#include "MultiplayerSession.h"

MultiplayerSession::MultiplayerSession(SocketIOSession* session)
	: SocketIOSession(std::move(*session))
{
}
