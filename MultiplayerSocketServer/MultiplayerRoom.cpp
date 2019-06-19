#include "MultiplayerRoom.h"
#include "MultiplayerSession.h"
#include "MultiplayerException.h"

#include <boost/thread/lock_guard.hpp>

MultiplayerRoom::MultiplayerRoom(const std::size_t& max_room_size)
	: max_room_size_(max_room_size)
{

}

void MultiplayerRoom::AddPlayer(std::shared_ptr<MultiplayerSession> player)
{
	boost::lock_guard<MultiplayerRoom> guard(*this);
	if (players_.size() >= get_max_room_size())
	{
		throw MultiplayerException("too many players in this room");
	}
	MultiplayerRoomSession session = {};
	session.player = player;
	session.disconnect_connection = player->ConnectOnDisconnect(
		[this](auto session) {
			RemovePlayer(session->get_id());
		});

	players_[player->get_id()] = std::move(session);
}

void MultiplayerRoom::RemovePlayer(const boost::uuids::uuid& player_id)
{
	boost::lock_guard<MultiplayerRoom> guard(*this);
	players_.erase(player_id);
}
