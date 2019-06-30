#include "MultiplayerRoomMessageHandler.h"

MultiplayerRoomMessageHandler::MultiplayerRoomMessageHandler(multiplayer_room_t room)
	: room_(room)
{
}

#pragma region LobbyMessageHandler
LobbyMessageHandler::LobbyMessageHandler(multiplayer_room_t room)
	: MultiplayerRoomMessageHandler(room)
{
}

void LobbyMessageHandler::HandleMessage(socket_io_session_t session, message_t message)
{
	auto wrapper_message = std::dynamic_pointer_cast<wrapper_message_t>(message);
	if (!wrapper_message)
	{
		return;
	}

	if (wrapper_message->has_authenticate())
	{
		//TODO check in with aws cognito
	}

}
#pragma endregion LobbyMessageHandler

#pragma region GameMessageHandler
GameMessageHandler::GameMessageHandler(multiplayer_room_t room)
	: MultiplayerRoomMessageHandler(room)
{
}

void GameMessageHandler::HandleMessage(socket_io_session_t session, message_t message)
{
	auto wrapper_message = std::dynamic_pointer_cast<wrapper_message_t>(message);
	if (!wrapper_message)
	{
		return;
	}

	if (wrapper_message->has_move())
	{
		//TODO move player
	}
}
#pragma endregion GameMessageHandler