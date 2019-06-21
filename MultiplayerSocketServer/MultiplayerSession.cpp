#include "MultiplayerSession.h"

MultiplayerSession::MultiplayerSession(SocketIOSession* session)
	: SocketIOSession(std::move(*session))
{
}

MultiplayerSession::MultiplayerSession(boost::asio::ip::tcp::socket&& socket, std::shared_ptr<SocketIOSessionManager> session_manager)
	: SocketIOSession(std::move(socket), session_manager)
{
}
