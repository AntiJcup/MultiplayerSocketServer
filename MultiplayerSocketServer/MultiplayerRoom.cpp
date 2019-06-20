#include "MultiplayerRoom.h"
#include "MultiplayerSession.h"
#include "MultiplayerException.h"

#include <boost/thread/lock_guard.hpp>

MultiplayerRoom::MultiplayerRoom(const std::size_t& max_room_size, boost::beast::net::io_context& io_context)
	: max_room_size_(max_room_size), io_context_(io_context)
{

}

void MultiplayerRoom::Initialize()
{

}

std::size_t MultiplayerRoom::get_player_count()
{
	boost::lock_guard<MultiplayerRoom> guard(*this);
	return players_.size();
}

void MultiplayerRoom::AddPlayer(std::shared_ptr<MultiplayerSession> player)
{
	boost::lock_guard<MultiplayerRoom> guard(*this);
	if (players_.size() >= get_max_room_size())
	{
		throw MultiplayerRoomException("too many players in this room", MultiplayerRoomExceptionType::RoomFull);
	}

	MultiplayerRoomSession session = {};
	session.player = player;
	session.disconnect_connection = player->ListenToDisconnect(
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

void MultiplayerRoom::Broadcast(std::shared_ptr<google::protobuf::MessageLite> message)
{
	boost::lock_guard<MultiplayerRoom> guard(*this);

	for (auto& session : players_)
	{
		session.second.player->Send(*message);
	}
}

void MultiplayerRoom::Send(const boost::uuids::uuid& player_id, std::shared_ptr<google::protobuf::MessageLite> message)
{
	boost::lock_guard<MultiplayerRoom> guard(*this);
	players_[player_id].player->Send(*message);
}

boost::signals2::connection MultiplayerRoom::ListenToComplete(const complete_signal_t::slot_type& subscriber)
{
	return complete_sig_.connect(subscriber);
}

void MultiplayerRoom::OnComplete()
{
	complete_sig_(this);
}
