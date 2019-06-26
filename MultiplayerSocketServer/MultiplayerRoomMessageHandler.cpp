#include "MultiplayerRoomMessageHandler.h"

MultiplayerRoomMessageHandler::MultiplayerRoomMessageHandler(std::shared_ptr<MultiplayerRoom> room)
	: room_(room)
{
}

#pragma region LobbyMessageHandler
LobbyMessageHandler::LobbyMessageHandler(std::shared_ptr<MultiplayerRoom> room)
	: MultiplayerRoomMessageHandler(room)
{
}

void LobbyMessageHandler::HandleMessage(std::shared_ptr<SocketIOSession> session, std::shared_ptr<google::protobuf::MessageLite> message)
{
	//switch (room_->get_room_sub_state())
	//{
	//case RoomSubState::Waiting:
	//case RoomSubState::Done:
	//	break;
	//default:
	//	return;
	//}

	auto wrapper_message = std::dynamic_pointer_cast<google::protobuf::WrapperMessage>(message);
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
GameMessageHandler::GameMessageHandler(std::shared_ptr<MultiplayerRoom> room)
	: MultiplayerRoomMessageHandler(room)
{
}

void GameMessageHandler::HandleMessage(std::shared_ptr<SocketIOSession> session, std::shared_ptr<google::protobuf::MessageLite> message)
{
	/*witch (room_->get_room_sub_state())
	{
	case RoomSubState::Waiting:
	case RoomSubState::Done:
		break;
	default:
		return;
	}*/

	auto wrapper_message = std::dynamic_pointer_cast<google::protobuf::WrapperMessage>(message);
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