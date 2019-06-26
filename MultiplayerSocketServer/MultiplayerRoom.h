#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>
#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/mutex.hpp>
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
	std::shared_ptr<MultiplayerSession> player;
};

class MultiplayerRoom_ :
	public boost::msm::front::state_machine_def<MultiplayerRoom_>,
	public boost::basic_lockable_adapter<boost::mutex>,
	public std::enable_shared_from_this<MultiplayerRoom_>
{
public:
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom_>)> complete_signal_t;
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom_>, ErrorCode, const char*)> error_signal_t;
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom_>)> start_signal_t;

#pragma region States
	struct Lobby_ : public boost::msm::front::state_machine_def<Lobby_>
	{
		struct WaitingForPlayers : public boost::msm::front::state<>
		{

		};

		struct Full : public boost::msm::front::state<>
		{

		};
		// optional entry/exit methods
		//template <class Event,class FSM>
		//void on_entry(Event const&,FSM& ) {std::cout << "entering: Waiting" << std::endl;}
		//template <class Event,class FSM>
		//void on_exit(Event const&,FSM& ) {std::cout << "leaving: Waiting" << std::endl;}

		typedef WaitingForPlayers initial_state;
	};
	typedef boost::msm::back::state_machine<Lobby_> Lobby;

	struct Game_ : public boost::msm::front::state_machine_def<Game_>
	{
		struct Loading : public boost::msm::front::state<>
		{

		};

		struct Playing : public boost::msm::front::state<>
		{

		};
		// optional entry/exit methods
		//template <class Event,class FSM>
		//void on_entry(Event const&,FSM& ) {std::cout << "entering: Waiting" << std::endl;}
		//template <class Event,class FSM>
		//void on_exit(Event const&,FSM& ) {std::cout << "leaving: Waiting" << std::endl;}

		typedef Loading initial_state;
	};
	typedef boost::msm::back::state_machine<Game_> Game;

	struct Closed : public boost::msm::front::state<>
	{
		// optional entry/exit methods
		//template <class Event,class FSM>
		//void on_entry(Event const&,FSM& ) {std::cout << "entering: Waiting" << std::endl;}
		//template <class Event,class FSM>
		//void on_exit(Event const&,FSM& ) {std::cout << "leaving: Waiting" << std::endl;}
	};

	typedef Lobby initial_state;
#pragma endregion States

#pragma region Events
	struct StartGame {};
	struct ReadyGame {};
	struct FinishGame {};
	struct ErrorLobby {};
#pragma endregion Events

#pragma region TransitionTable
	// Transition table for Playing
	struct transition_table : boost::mpl::vector<
		//								Start       Event        Next            Action                 Guard
		//							+---------+--------------+---------+------------------------+----------------------+
		boost::msm::front::Row <		Lobby, StartGame, Game, boost::msm::front::none, boost::msm::front::none			>,
		boost::msm::front::Row <		Game, FinishGame, Closed, boost::msm::front::none, boost::msm::front::none		>,
		boost::msm::front::Row <		Lobby, ErrorLobby, Closed, boost::msm::front::none, boost::msm::front::none			>
	> {};
#pragma endregion TransitionTable

	MultiplayerRoom_(const std::size_t& max_room_size,
		const std::size_t& min_room_size,
		const std::size_t& max_start_time,
		std::shared_ptr<boost::beast::net::io_context> io_context);

	void Initialize();

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
	std::unordered_map<boost::uuids::uuid, MultiplayerRoomSession, boost::hash<boost::uuids::uuid>> sessions_;
	boost::uuids::uuid id_{ boost::uuids::random_generator()() };

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

typedef boost::msm::back::state_machine<MultiplayerRoom_> MultiplayerRoom;