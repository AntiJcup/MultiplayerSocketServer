#include "MultiplayerSessionManager.h"
#include <boost/thread/lock_guard.hpp>

static const std::size_t max_room_size = 4;

MultiplayerSessionManager::MultiplayerSessionManager(boost::beast::net::io_context& io_context)
	: io_context_(io_context)
{
}

void MultiplayerSessionManager::Initialize()
{
	NewLobby();
}

std::shared_ptr<SocketIOSession> MultiplayerSessionManager::CreateNewSession(boost::asio::ip::tcp::socket&& socket)
{
	auto new_session =
		SocketIOSessionManager::CreateNewSession(std::move(socket));

	if (!new_session)
	{
		return new_session;
	}

	auto new_player = std::make_shared<MultiplayerSession>(new_session.get());

	{
		std::shared_ptr<MultiplayerRoom> lobby_room;
		bool room_full = false;
		{
			boost::lock_guard<MultiplayerSessionManager> guard(*this);
			lobby_room = get_current_lobby();
			room_full = lobby_room->get_player_count() >= lobby_room->get_max_room_size();
		}

		if (room_full)
		{
			lobby_room = NewLobby();
		}

		lobby_room->AddPlayer(new_player);
	}

	return std::static_pointer_cast<SocketIOSession>(new_player);
}

std::shared_ptr<MultiplayerRoom> MultiplayerSessionManager::NewLobby()
{
	auto new_room = std::make_shared<MultiplayerRoom>(max_room_size, io_context_);
	AddRoom(new_room);
	set_current_lobby(new_room);

	return new_room;
}

void MultiplayerSessionManager::AddRoom(std::shared_ptr<MultiplayerRoom> room)
{
	boost::lock_guard<MultiplayerSessionManager> guard(*this);

	MultiplayerRoomManagerSession session = {};
	session.room = room;
	session.complete_connection = room->ListenToComplete(
		[this](auto room) {
			RemoveRoom(room->get_id());
		});

	rooms_[room->get_id()] = std::move(session);
}

void MultiplayerSessionManager::RemoveRoom(boost::uuids::uuid room_id)
{
	boost::lock_guard<MultiplayerSessionManager> guard(*this);
	rooms_.erase(room_id);
}
