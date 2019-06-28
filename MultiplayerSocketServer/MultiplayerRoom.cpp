#include "MultiplayerRoom.h"
#include "MultiplayerSession.h"
#include "MultiplayerException.h"

#include <boost/thread/lock_guard.hpp>

MultiplayerRoom_::MultiplayerRoom_(const std::size_t& max_room_size,
	const std::size_t& min_room_size,
	const std::size_t& max_start_time,
	std::shared_ptr<boost::beast::net::io_context> io_context)
	: max_room_size_(max_room_size),
	min_room_size_(min_room_size),
	max_start_time_(max_start_time),
	io_context_(io_context),
	start_timer_(*io_context)
{

}

void MultiplayerRoom_::Initialize()
{
	boost::lock_guard<MultiplayerRoom_> guard(*this);
	//message_handlers_[RoomState::Lobby] = std::shared_ptr<MultiplayerRoomMessageHandler>(new LobbyMessageHandler(shared_from_this()));
}

#pragma region Getters
std::size_t MultiplayerRoom_::get_player_count()
{
	boost::lock_guard<MultiplayerRoom_> guard(*this);
	return sessions_.size();
}

std::vector<boost::uuids::uuid> MultiplayerRoom_::get_player_ids()
{
	std::vector<boost::uuids::uuid> player_ids(sessions_.size());
	boost::lock_guard<MultiplayerRoom_> guard(*this);
	for (auto& player : sessions_)
	{
		player_ids.push_back(player.second.player->get_id());
	}

	return player_ids;
}
#pragma endregion Getters

#pragma region Sessions
void MultiplayerRoom_::AddSession(std::shared_ptr<MultiplayerSession> player)
{
	boost::lock_guard<MultiplayerRoom_> guard(*this);
	if (sessions_.size() >= get_max_room_size())
	{
		throw MultiplayerRoomException("too many players in this room", MultiplayerRoomExceptionType::RoomFull);
	}

	MultiplayerRoomSession session = {};
	session.player = player;
	session.disconnect_connection = player->ListenToDisconnect(
		[this](auto session) {
			io_context_->post(boost::bind(&MultiplayerRoom_::RemoveSession, this, session->get_id()));
		});
	session.message_connection = player->ListenToMessages(
		[this](auto session, auto message) {
			io_context_->post(boost::bind(&MultiplayerRoom_::OnMessage, this, session, message));
		});
	sessions_[player->get_id()] = std::move(session);

	if (sessions_.size() >= get_min_room_size())
	{
		start_timer_.expires_from_now(boost::posix_time::seconds(get_max_start_time()));
		start_timer_.async_wait([this](auto error_code)
			{
				io_context_->post(boost::bind(&MultiplayerRoom_::OnStartTimer, this));
			});
	}
	else if (sessions_.size() >= get_max_room_size())
	{
		start_timer_.cancel();
		io_context_->post(boost::bind(&MultiplayerRoom_::OnStartTimer, this));
	}
}

std::shared_ptr<MultiplayerSession> MultiplayerRoom_::RemoveSession(const boost::uuids::uuid& player_id)
{
	auto error = false;
	std::shared_ptr<MultiplayerSession> player;
	{
		boost::lock_guard<MultiplayerRoom_> guard(*this);
		player = sessions_[player_id].player;
		sessions_.erase(player_id);

		error = sessions_.size() < get_min_room_size();
	}

	if (error)
	{
		OnError(ErrorCode::MinRoomSizeLost, "Player left");
	}

	return player;
}
#pragma endregion Sessions

#pragma region Messaging
void MultiplayerRoom_::Broadcast(std::shared_ptr<google::protobuf::MessageLite> message)
{
	boost::lock_guard<MultiplayerRoom_> guard(*this);

	for (auto& session : sessions_)
	{
		session.second.player->Send(*message);
	}
}

void MultiplayerRoom_::Send(const boost::uuids::uuid& player_id, std::shared_ptr<google::protobuf::MessageLite> message)
{
	boost::lock_guard<MultiplayerRoom_> guard(*this);
	sessions_[player_id].player->Send(*message);
}

void MultiplayerRoom_::OnMessage(std::shared_ptr<SocketIOSession> session, std::shared_ptr<google::protobuf::MessageLite> message)
{
	//message_handlers_[RoomState::Lobby]->HandleMessage(session, message);
}
#pragma endregion Messaging

#pragma region Listen
boost::signals2::connection MultiplayerRoom_::ListenToComplete(const complete_signal_t::slot_type& subscriber)
{
	return complete_sig_.connect(subscriber);
}

boost::signals2::connection MultiplayerRoom_::ListenToError(const error_signal_t::slot_type& subscriber)
{
	return error_sig_.connect(subscriber);
}

boost::signals2::connection MultiplayerRoom_::ListenToStart(const start_signal_t::slot_type& subscriber)
{
	return start_sig_.connect(subscriber);
}

void MultiplayerRoom_::OnComplete()
{
	complete_sig_(shared_from_this());
}

void MultiplayerRoom_::OnError(ErrorCode error, const char* msg)
{
	error_sig_(shared_from_this(), error, msg);
}

void MultiplayerRoom_::OnStartTimer()
{
	start_sig_(shared_from_this());
	//set_room_state(RoomState::Game);
}
#pragma endregion Listen