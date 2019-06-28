#include "MultiplayerSession.h"

MultiplayerSession_::MultiplayerSession_(SocketIOSession* session)
	: SocketIOSession(std::move(*session))
{
}

MultiplayerSession_::MultiplayerSession_(boost::asio::ip::tcp::socket &socket, std::shared_ptr<SocketIOSessionManager> session_manager)
	: SocketIOSession(std::move(socket), session_manager)
{
}
