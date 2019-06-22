#include "MultiplayerSessionManager.h"
#include <boost/thread/lock_guard.hpp>

static const std::size_t max_room_size = 4;
static const std::size_t min_room_size = 2;
static const std::size_t max_room_wait = 5;

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
	auto session = std::make_shared<MultiplayerSession>(std::move(socket), shared_from_this());
	session->Run();
	{
		boost::lock_guard<MultiplayerSessionManager> guard(*this);
		SocketIOSessionWrapper session_wrapper = {};
		session_wrapper.session = session;
		session_wrapper.disconnect_connection = session->ListenToDisconnect(
			[this](auto session) {
				RemoveSession(session->get_id());
			});
		sessions_[session->get_id()] = std::move(session_wrapper);
	}

	AddPlayerToCurrentLobbyRoom(session);
	return session;
}

std::shared_ptr<MultiplayerRoom> MultiplayerSessionManager::NewLobby()
{
	auto new_room = std::make_shared<MultiplayerRoom>(max_room_size, min_room_size, max_room_wait, io_context_);
	AddRoom(new_room);
	new_room->Initialize();
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

	session.error_connection = room->ListenToError(
		[this](auto room, auto error, auto msg) {
			RemoveRoom(room->get_id());
		});

	session.start_connection = room->ListenToStart(
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

void MultiplayerSessionManager::TransportPlayersToActiveLobby(boost::uuids::uuid room_id)
{
	std::vector<boost::uuids::uuid> player_ids;
	std::shared_ptr<MultiplayerRoom> room;
	{
		boost::lock_guard<MultiplayerSessionManager> guard(*this);
		room = rooms_[room_id].room;
		player_ids = std::move(room->get_player_ids());
	}

	for (auto& player_id : player_ids)
	{
		room->RemovePlayer(player_id);
		AddPlayerToCurrentLobbyRoom(std::dynamic_pointer_cast<MultiplayerSession>(sessions_[player_id].session));
	}
}

void MultiplayerSessionManager::AddPlayerToCurrentLobbyRoom(std::shared_ptr<MultiplayerSession> player)
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

	lobby_room->AddPlayer(player);
}
