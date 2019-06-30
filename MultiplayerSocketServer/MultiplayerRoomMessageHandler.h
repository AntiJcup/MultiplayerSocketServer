#pragma once
#include "SocketIOSession.h"
#include "MultiplayerCommonTypes.h"
#include "MultiplayerRoom.h"

class MultiplayerRoomMessageHandler
{
public:
	MultiplayerRoomMessageHandler(multiplayer_room_t room);

	virtual void HandleMessage(socket_io_session_t session, message_t message) = 0;

protected:
	multiplayer_room_t room_;
};

class LobbyMessageHandler : public MultiplayerRoomMessageHandler
{
public:
	LobbyMessageHandler(multiplayer_room_t room);

	virtual void HandleMessage(socket_io_session_t session, message_t message) override;
};

class GameMessageHandler : public MultiplayerRoomMessageHandler
{
public:
	GameMessageHandler(multiplayer_room_t room);

	virtual void HandleMessage(socket_io_session_t session, message_t message) override;
};