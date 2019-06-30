#include "MultiplayerRoom.h"
#include "MultiplayerSession.h"
#include "MultiplayerException.h"


MultiplayerRoom::MultiplayerRoom(const std::size_t& max_room_size,
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

void MultiplayerRoom::Initialize()
{
	boost::lock_guard<MultiplayerRoom> guard(*this);
	//message_handlers_[RoomState::Lobby] = std::shared_ptr<MultiplayerRoomMessageHandler>(new LobbyMessageHandler(shared_from_this()));
}

#pragma region Getters
std::size_t MultiplayerRoom::get_player_count()
{
	boost::lock_guard<MultiplayerRoom> guard(*this);
	return sessions_.size();
}

std::vector<socket_io_session_id_t> MultiplayerRoom::get_player_ids()
{
	std::vector<socket_io_session_id_t> player_ids(sessions_.size());
	boost::lock_guard<MultiplayerRoom> guard(*this);
	for (auto& player : sessions_)
	{
		player_ids.push_back(player.second.player->get_id());
	}

	return player_ids;
}
#pragma endregion Getters

#pragma region Sessions
void MultiplayerRoom::AddSession(multiplayer_session_t player)
{
	boost::lock_guard<MultiplayerRoom> guard(*this);
	if (sessions_.size() >= get_max_room_size())
	{
		throw MultiplayerRoomException("too many players in this room", MultiplayerRoomExceptionType::RoomFull);
	}

	MultiplayerRoomSession session = {};
	session.player = player;
	session.disconnect_connection = player->ListenToDisconnect(
		[this](auto session) {
			io_context_->post(boost::bind(&MultiplayerRoom::RemoveSession, this, session->get_id()));
		});
	session.message_connection = player->ListenToMessages(
		[this](auto session, auto message) {
			io_context_->post(boost::bind(&MultiplayerRoom::OnMessage, this, session, message));
		});
	sessions_[player->get_id()] = std::move(session);

	if (sessions_.size() >= get_min_room_size())
	{
		start_timer_.expires_from_now(boost::posix_time::seconds(get_max_start_time()));
		start_timer_.async_wait([this](auto error_code)
			{
				io_context_->post(boost::bind(&MultiplayerRoom::OnStartTimer, this));
			});
	}
	else if (sessions_.size() >= get_max_room_size())
	{
		start_timer_.cancel();
		io_context_->post(boost::bind(&MultiplayerRoom::OnStartTimer, this));
	}
}

void MultiplayerRoom::RemoveSession(const socket_io_session_id_t& player_id)
{
	auto error = false;
	std::shared_ptr<MultiplayerSession> player;
	{
		boost::lock_guard<MultiplayerRoom> guard(*this);
		player = sessions_[player_id].player;
		sessions_.erase(player_id);

		error = sessions_.size() < get_min_room_size();
	}

	if (error)
	{
		OnError(ErrorCode::MinRoomSizeLost, "Player left");
	}
}
#pragma endregion Sessions

#pragma region Messaging
void MultiplayerRoom::Broadcast(message_t message)
{
	boost::lock_guard<MultiplayerRoom> guard(*this);

	for (auto& session : sessions_)
	{
		session.second.player->Send(message);
	}
}

void MultiplayerRoom::Send(const socket_io_session_id_t& player_id, message_t message)
{
	boost::lock_guard<MultiplayerRoom> guard(*this);
	sessions_[player_id].player->Send(message);
}

void MultiplayerRoom::OnMessage(std::shared_ptr<SocketIOSession> session, std::shared_ptr<google::protobuf::MessageLite> message)
{
	//message_handlers_[RoomState::Lobby]->HandleMessage(session, message);
}
#pragma endregion Messaging

#pragma region Listen
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
	complete_sig_(shared_from_this());
}

void MultiplayerRoom::OnError(ErrorCode error, const char* msg)
{
	error_sig_(shared_from_this(), error, msg);
}

void MultiplayerRoom::OnStartTimer()
{
	start_sig_(shared_from_this());
}
#pragma endregion Listen