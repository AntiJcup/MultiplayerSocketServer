#include "MultiplayerRoom.h"
#include "MultiplayerSession.h"
#include "MultiplayerException.h"

#include <boost/thread/lock_guard.hpp>

MultiplayerRoom::MultiplayerRoom(const std::size_t& max_room_size,
	const std::size_t& min_room_size,
	const std::size_t& max_start_time,
	boost::beast::net::io_context& io_context)
	: max_room_size_(max_room_size),
	min_room_size_(min_room_size),
	max_start_time_(max_start_time),
	io_context_(io_context),
	start_timer_(io_context)
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

std::vector<boost::uuids::uuid> MultiplayerRoom::get_player_ids()
{
	std::vector<boost::uuids::uuid> player_ids(players_.size());
	boost::lock_guard<MultiplayerRoom> guard(*this);
	for (auto& player : players_)
	{
		player_ids.push_back(player.second.player->get_id());
	}

	return player_ids;
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

	if (players_.size() >= get_min_room_size())
	{
		start_timer_.expires_from_now(boost::posix_time::seconds(get_max_start_time()));
		start_timer_.async_wait([this](auto error_code)
			{
				OnStartTimer();
			});
	}
	else if (players_.size() >= get_max_room_size())
	{
		start_timer_.cancel();
		OnStartTimer();
	}
}

std::shared_ptr<MultiplayerSession> MultiplayerRoom::RemovePlayer(const boost::uuids::uuid& player_id)
{
	boost::lock_guard<MultiplayerRoom> guard(*this);
	auto player = players_[player_id].player;
	players_.erase(player_id);

	if (players_.size() < get_min_room_size() && get_room_state() == RoomState::Game)
	{
		OnError(ErrorCode::MinRoomSizeLost, "Player left");
	}

	return player;
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

boost::signals2::connection MultiplayerRoom::ListenToError(const error_signal_t::slot_type& subscriber)
{
	return error_sig_.connect(subscriber);
}

boost::signals2::connection MultiplayerRoom::ListenToStart(const start_signal_t::slot_type& subscriber)
{
	return start_sig_.connect(subscriber);
}

void MultiplayerRoom::OnComplete()
{
	complete_sig_(this);
}

void MultiplayerRoom::OnError(ErrorCode error, const char* msg)
{
	error_sig_(this, error, msg);
}

void MultiplayerRoom::OnStartTimer()
{
	start_sig_(this);
	set_room_state(RoomState::Game);
}
