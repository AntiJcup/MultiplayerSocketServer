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
	std::shared_ptr<MultiplayerSession> player;
};

class MultiplayerRoom_ :
	public boost::msm::front::state_machine_def<MultiplayerRoom_>,
	public boost::basic_lockable_adapter<boost::recursive_mutex>,
	public std::enable_shared_from_this<MultiplayerRoom_>
{
	typedef std::unordered_map<socket_io_session_id_t, MultiplayerRoomSession, socket_io_session_id_hasher_t> session_map_t;
public:
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom_>)> complete_signal_t;
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom_>, ErrorCode, const char*)> error_signal_t;
	typedef boost::signals2::signal<void(std::shared_ptr<MultiplayerRoom_>)> start_signal_t;

#pragma region Events
	//Top
	struct StartGame {};
	struct ReadyGame {};
	struct FinishGame {};
	struct Error {};

	//Internal
	struct MessageReceived
	{
		MessageReceived(std::shared_ptr<SocketIOSession> session, std::shared_ptr<google::protobuf::MessageLite> message)
			: session(session), message(message)
		{

		}

		std::shared_ptr<SocketIOSession> session;
		std::shared_ptr<google::protobuf::MessageLite> message;
	};

	struct SessionDisconnect
	{
		socket_io_session_id_t session_id;
	};

	struct SessionConnect
	{
		multiplayer_session_t session;
	};
#pragma endregion Events

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

		struct internal_guard_fct
		{
			template <class EVT, class FSM, class SourceState, class TargetState>
			bool operator()(EVT const& evt, FSM&, SourceState&, TargetState&)
			{
				return false;
			}
		};

		struct internal_on_message_fct
		{
			template <class EVT, class FSM, class SourceState, class TargetState>
			void operator()(EVT const&, FSM&, SourceState&, TargetState&)
			{
				std::cout << "Empty::internal_transition_table internal_on_message_fct action" << std::endl;
			}
		};

		struct internal_on_session_connect_fct
		{
			template <class EVT, class FSM, class SourceState, class TargetState>
			void operator()(EVT const&, FSM&, SourceState&, TargetState&)
			{
				std::cout << "Empty::internal_transition_table internal_on_session_connect_fct action" << std::endl;
			}
		};

		struct internal_on_session_disconnect_fct
		{
			template <class EVT, class FSM, class SourceState, class TargetState>
			void operator()(EVT const&, FSM&, SourceState&, TargetState&)
			{
				std::cout << "Empty::internal_transition_table internal_on_session_disconnect_fct action" << std::endl;
			}
		};

		struct internal_transition_table : boost::mpl::vector<
			//								Start       Event        Next            Action                 Guard
			//							+---------+--------------+---------+------------------------+----------------------+
			boost::msm::front::Internal <	 MessageReceived, internal_on_message_fct, internal_guard_fct			>,
			boost::msm::front::Internal <	 SessionDisconnect, internal_on_session_disconnect_fct, internal_guard_fct			>,
			boost::msm::front::Internal <	 SessionConnect, internal_on_session_connect_fct, internal_guard_fct			>
		> {};

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

#pragma region TransitionTable
	// Transition table for Playing
	struct transition_table : boost::mpl::vector<
		//								Start       Event        Next            Action                 Guard
		//							+---------+--------------+---------+------------------------+----------------------+
		boost::msm::front::Row <		Lobby, StartGame, Game, boost::msm::front::none, boost::msm::front::none			>,
		boost::msm::front::Row <		Game, FinishGame, Closed, boost::msm::front::none, boost::msm::front::none		>,
		boost::msm::front::Row <		Lobby, Error, Closed, boost::msm::front::none, boost::msm::front::none			>
	> {};
#pragma endregion TransitionTable

	MultiplayerRoom_(const std::size_t& max_room_size,
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

	void AddSession(std::shared_ptr<MultiplayerSession> player);
	void RemoveSession(const socket_io_session_id_t& player_id);

	void Broadcast(message_t message);
	void Send(const socket_io_session_id_t& player_id, message_t message);

	boost::signals2::connection ListenToComplete(const complete_signal_t::slot_type& subscriber);
	boost::signals2::connection ListenToError(const error_signal_t::slot_type& subscriber);
	boost::signals2::connection ListenToStart(const start_signal_t::slot_type& subscriber);

	template<class Event>
	void ProcessEvent(const Event& ev)
	{
		boost::lock_guard<MultiplayerRoom_> guard(*this);
		boost::msm::back::state_machine<MultiplayerRoom_>& fsm = static_cast<boost::msm::back::state_machine<MultiplayerRoom_>&>(*this);
		fsm.process_event(ev);
	}
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

	void AddSessionInternal(std::shared_ptr<MultiplayerSession> player);
	void RemoveSessionInternal(const socket_io_session_id_t& player_id);

	friend class MultiplayerRoomMessageHandler;
};

typedef boost::msm::back::state_machine<MultiplayerRoom_> MultiplayerRoom;
typedef std::shared_ptr<MultiplayerRoom> multiplayer_room_t;