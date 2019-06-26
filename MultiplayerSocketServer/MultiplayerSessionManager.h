#pragma once

#include "SocketIOSessionManager.h"
#include "MultiplayerRoom.h"
#include <boost/beast/core.hpp>

class MultiplayerRoomManagerSession
{
public:
	boost::signals2::scoped_connection error_connection;
	boost::signals2::scoped_connection complete_connection;
	boost::signals2::scoped_connection start_connection;
	std::shared_ptr<MultiplayerRoom> room;
};

class MultiplayerSessionManager : public SocketIOSessionManager
{
public:
	MultiplayerSessionManager(std::shared_ptr<boost::beast::net::io_context> io_context);
	void Initialize();

	virtual std::shared_ptr<SocketIOSession> CreateNewSession(boost::asio::ip::tcp::socket&& socket) override;

	std::shared_ptr<MultiplayerRoom> NewLobby();

	void AddRoom(std::shared_ptr<MultiplayerRoom> room);

	void RemoveRoom(boost::uuids::uuid room_id);

	void TransportPlayersToActiveLobby(boost::uuids::uuid room_id);

	std::shared_ptr<MultiplayerRoom> get_current_lobby()
	{
		return current_lobby_;
	}

	void set_current_lobby(std::shared_ptr<MultiplayerRoom> room)
	{
		current_lobby_ = room;
	}

	void AddPlayerToCurrentLobbyRoom(std::shared_ptr<MultiplayerSession> player);
private:
	std::unordered_map<boost::uuids::uuid, MultiplayerRoomManagerSession, boost::hash<boost::uuids::uuid>> rooms_;
	std::shared_ptr<MultiplayerRoom> current_lobby_;
	std::shared_ptr<boost::beast::net::io_context> io_context_;
};

