#include "MultiplayerSession.h"

MultiplayerSession::MultiplayerSession(SocketIOSession* session)
	: SocketIOSession(std::move(*session))
{
}

MultiplayerSession::MultiplayerSession(boost::asio::ip::tcp::socket &socket, socket_io_session_manager_t session_manager)
	: SocketIOSession(std::move(socket), session_manager)
{
}
