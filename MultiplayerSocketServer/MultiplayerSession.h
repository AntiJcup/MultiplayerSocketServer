#pragma once

#include "SocketIOSession.h"
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/functor_row.hpp>

class MultiplayerSession_
	: public SocketIOSession,
	public boost::msm::front::state_machine_def<MultiplayerSession_>
{
public:
	MultiplayerSession_(SocketIOSession* session);

	MultiplayerSession_(boost::asio::ip::tcp::socket &socket, std::shared_ptr<SocketIOSessionManager> session_manager);

	struct Authenticating : public boost::msm::front::state<>
	{
		// optional entry/exit methods
		//template <class Event,class FSM>
		//void on_entry(Event const&,FSM& ) {std::cout << "entering: Waiting" << std::endl;}
		//template <class Event,class FSM>
		//void on_exit(Event const&,FSM& ) {std::cout << "leaving: Waiting" << std::endl;}
	};

	struct Ready : public boost::msm::front::state<>
	{
		// optional entry/exit methods
		//template <class Event,class FSM>
		//void on_entry(Event const&,FSM& ) {std::cout << "entering: Waiting" << std::endl;}
		//template <class Event,class FSM>
		//void on_exit(Event const&,FSM& ) {std::cout << "leaving: Waiting" << std::endl;}
	};

	struct Playing : public boost::msm::front::state<>
	{
		// optional entry/exit methods
		//template <class Event,class FSM>
		//void on_entry(Event const&,FSM& ) {std::cout << "entering: Waiting" << std::endl;}
		//template <class Event,class FSM>
		//void on_exit(Event const&,FSM& ) {std::cout << "leaving: Waiting" << std::endl;}
	};

	struct Disconnected : public boost::msm::front::state<>
	{
		// optional entry/exit methods
		//template <class Event,class FSM>
		//void on_entry(Event const&,FSM& ) {std::cout << "entering: Waiting" << std::endl;}
		//template <class Event,class FSM>
		//void on_exit(Event const&,FSM& ) {std::cout << "leaving: Waiting" << std::endl;}
	};

	typedef Authenticating initial_state;
};

typedef boost::msm::back::state_machine<MultiplayerSession_> MultiplayerSession;