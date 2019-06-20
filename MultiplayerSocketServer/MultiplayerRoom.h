#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/signals2/signal.hpp>

#include <unordered_map>
#include <memory>

#include "WrapperMessage.pb.h"

class MultiplayerSession;

enum class RoomState
{
	Lobby = 0,
	Game = 1,
};

enum class RoomSubState
{
	None = 0,
	Loading = 1,
	Starting = 2,
	Done = 3,
	Waiting = 4,
};

class MultiplayerRoomSession
{
public:
	boost::signals2::scoped_connection disconnect_connection;
	std::shared_ptr<MultiplayerSession> player;
};

class MultiplayerRoom :
	public boost::basic_lockable_adapter<boost::mutex>
{
public:
	MultiplayerRoom(const std::size_t& max_room_size);

	RoomState get_room_state() const
	{
		return room_state_;
	}

	void set_room_state(RoomState room_state)
	{
		room_state_ = room_state;
	}

	RoomSubState get_room_sub_state() const
	{
		return room_sub_state_;
	}

	void set_room_sub_state(RoomSubState room_sub_state)
	{
		room_sub_state_ = room_sub_state;
	}

	std::size_t get_max_room_size()
	{
		return max_room_size_;
	}

	void AddPlayer(std::shared_ptr<MultiplayerSession> player);
	void RemovePlayer(const boost::uuids::uuid& player_id);

	void Broadcast(std::shared_ptr<google::protobuf::MessageLite> message);
	void Send(const boost::uuids::uuid& session_id, std::shared_ptr<google::protobuf::MessageLite> message);

private:
	std::size_t max_room_size_;
	RoomState room_state_{ RoomState::Lobby };
	RoomSubState room_sub_state_{ RoomSubState::None };
	std::unordered_map<boost::uuids::uuid, MultiplayerRoomSession, boost::hash<boost::uuids::uuid>> players_;
};

