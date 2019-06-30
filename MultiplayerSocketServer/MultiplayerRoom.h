#pragma once
#include <boost/functional/hash.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/functor_row.hpp>

#include <unordered_map>
#include <memory>
#include <atomic>

#include "WrapperMessage.pb.h"
#include "MultiplayerSession.h"
#include "MultiplayerRoomMessageHandler.h"
#include "MultiplayerCommonTypes.h"
#include "SocketIOCommonTypes.h"

enum class ErrorCode
{
	MinRoomSizeLost,
};

class MultiplayerRoomMessageHandler;

class MultiplayerRoomSession
{
public:
	boost::signals2::scoped_connection disconnect_connection;
	boost::signals2::scoped_connection message_connection;
	multiplayer_session_t player;
};

class MultiplayerRoom :
	public boost::basic_lockable_adapter<boost::recursive_mutex>,
	public std::enable_shared_from_this<MultiplayerRoom>
{
	typedef std::unordered_map<socket_io_session_id_t, MultiplayerRoomSession, socket_io_session_id_hasher_t> session_map_t;
public:
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom>)> complete_signal_t;
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom>, ErrorCode, const char*)> error_signal_t;
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom>)> start_signal_t;

	MultiplayerRoom(const std::size_t& max_room_size,
		const std::size_t& min_room_size,
		const std::size_t& max_start_time,
		std::shared_ptr<boost::beast::net::io_context> io_context);

	void Initialize();

	const std::size_t& get_max_room_size() const noexcept
	{
		return max_room_size_;
	}

	const std::size_t& get_min_room_size() const noexcept
	{
		return min_room_size_;
	}

	const std::size_t& get_max_start_time() const noexcept
	{
		return max_start_time_;
	}

	const multiplayer_room_id_t& get_id() const noexcept
	{
		return id_;
	}

	std::size_t get_player_count();

	std::vector<socket_io_session_id_t> get_player_ids();

	void AddSession(multiplayer_session_t player);
	void RemoveSession(const socket_io_session_id_t& player_id);

	void Broadcast(message_t message);
	void Send(const socket_io_session_id_t& player_id, message_t message);

	boost::signals2::connection ListenToComplete(const complete_signal_t::slot_type& subscriber);
	boost::signals2::connection ListenToError(const error_signal_t::slot_type& subscriber);
	boost::signals2::connection ListenToStart(const start_signal_t::slot_type& subscriber);

private:
	std::size_t max_room_size_;
	std::size_t min_room_size_;
	std::size_t max_start_time_;
	session_map_t sessions_;
	multiplayer_room_id_t id_{ boost::uuids::random_generator()() };

	boost::asio::deadline_timer start_timer_;

	complete_signal_t complete_sig_;
	error_signal_t error_sig_;
	start_signal_t start_sig_;

	std::shared_ptr<boost::beast::net::io_context> io_context_;

	void OnComplete();
	void OnError(ErrorCode error, const char* msg);
	void OnStartTimer();

	void OnMessage(std::shared_ptr<SocketIOSession> session, std::shared_ptr<google::protobuf::MessageLite> message);

	friend class MultiplayerRoomMessageHandler;
};

