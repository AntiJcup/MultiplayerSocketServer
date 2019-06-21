#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/core.hpp>

#include <unordered_map>
#include <memory>

#include "WrapperMessage.pb.h"
#include "MultiplayerSession.h"

enum class RoomState
{
	Lobby = 0,
	Game = 1,
	Complete = 2,
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
	typedef boost::signals2::signal<void(MultiplayerRoom*)> complete_signal_t;
	typedef boost::signals2::signal<void(MultiplayerRoom*)> error_signal_t;

	MultiplayerRoom(const std::size_t& max_room_size,
		const std::size_t& min_room_size,
		boost::beast::net::io_context& io_context);

	void Initialize();

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

	boost::uuids::uuid get_id()
	{
		return id_;
	}

	std::size_t get_player_count();

	std::vector<boost::uuids::uuid> get_player_ids();

	void AddPlayer(std::shared_ptr<MultiplayerSession> player);
	std::shared_ptr<MultiplayerSession> RemovePlayer(const boost::uuids::uuid& player_id);

	void Broadcast(std::shared_ptr<google::protobuf::MessageLite> message);
	void Send(const boost::uuids::uuid& player_id, std::shared_ptr<google::protobuf::MessageLite> message);

	boost::signals2::connection ListenToComplete(const complete_signal_t::slot_type& subscriber);
	boost::signals2::connection ListenToError(const error_signal_t::slot_type& subscriber);

private:
	std::size_t max_room_size_;
	std::size_t min_room_size_;
	RoomState room_state_{ RoomState::Lobby };
	RoomSubState room_sub_state_{ RoomSubState::None };
	std::unordered_map<boost::uuids::uuid, MultiplayerRoomSession, boost::hash<boost::uuids::uuid>> players_;
	boost::uuids::uuid id_{ boost::uuids::random_generator()() };

	complete_signal_t complete_sig_;
	error_signal_t error_sig_;

	boost::beast::net::io_context& io_context_;

	void OnComplete();
	void OnError();
};

