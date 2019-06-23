#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>

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

enum class ErrorCode
{
	MinRoomSizeLost,
};

class MultiplayerRoomSession
{
public:
	boost::signals2::scoped_connection disconnect_connection;
	boost::signals2::scoped_connection message_connection;
	std::shared_ptr<MultiplayerSession> player;
};

class MultiplayerRoom :
	public boost::basic_lockable_adapter<boost::mutex>,
	public std::enable_shared_from_this<MultiplayerRoom>
{
public:
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom>)> complete_signal_t;
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom>, ErrorCode, const char*)> error_signal_t;
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom>)> start_signal_t;

	MultiplayerRoom(const std::size_t& max_room_size,
		const std::size_t& min_room_size,
		const std::size_t& max_start_time,
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

	const std::size_t& get_max_room_size()
	{
		return max_room_size_;
	}

	const std::size_t& get_min_room_size()
	{
		return min_room_size_;
	}

	const std::size_t& get_max_start_time()
	{
		return max_start_time_;
	}

	boost::uuids::uuid get_id()
	{
		return id_;
	}

	std::size_t get_player_count();

	std::vector<boost::uuids::uuid> get_player_ids();

	void AddSession(std::shared_ptr<MultiplayerSession> player);
	std::shared_ptr<MultiplayerSession> RemoveSession(const boost::uuids::uuid& player_id);

	void Broadcast(std::shared_ptr<google::protobuf::MessageLite> message);
	void Send(const boost::uuids::uuid& player_id, std::shared_ptr<google::protobuf::MessageLite> message);

	boost::signals2::connection ListenToComplete(const complete_signal_t::slot_type& subscriber);
	boost::signals2::connection ListenToError(const error_signal_t::slot_type& subscriber);
	boost::signals2::connection ListenToStart(const start_signal_t::slot_type& subscriber);

private:
	std::size_t max_room_size_;
	std::size_t min_room_size_;
	std::size_t max_start_time_;
	RoomState room_state_{ RoomState::Lobby };
	RoomSubState room_sub_state_{ RoomSubState::None };
	std::unordered_map<boost::uuids::uuid, MultiplayerRoomSession, boost::hash<boost::uuids::uuid>> sessions_;
	boost::uuids::uuid id_{ boost::uuids::random_generator()() };

	boost::asio::deadline_timer start_timer_;

	complete_signal_t complete_sig_;
	error_signal_t error_sig_;
	start_signal_t start_sig_;

	boost::beast::net::io_context& io_context_;

	void OnComplete();
	void OnError(ErrorCode error, const char* msg);
	void OnStartTimer();

	void OnMessage(std::shared_ptr<SocketIOSession> session, std::shared_ptr<google::protobuf::MessageLite> message);
};

