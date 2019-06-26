#pragma once
#include "SocketIOSession.h"
#include <google/protobuf/message_lite.h>

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>
#include "MultiplayerRoom.h"

class MultiplayerRoom_;
typedef boost::msm::back::state_machine<MultiplayerRoom_> MultiplayerRoom;

class MultiplayerRoomMessageHandler
{
public:
	MultiplayerRoomMessageHandler(std::shared_ptr<MultiplayerRoom> room);

	virtual void HandleMessage(std::shared_ptr<SocketIOSession> session, std::shared_ptr<google::protobuf::MessageLite> message) = 0;

protected:
	std::shared_ptr<MultiplayerRoom> room_;
};

class LobbyMessageHandler : public MultiplayerRoomMessageHandler
{
public:
	LobbyMessageHandler(std::shared_ptr<MultiplayerRoom> room);

	virtual void HandleMessage(std::shared_ptr<SocketIOSession> session, std::shared_ptr<google::protobuf::MessageLite> message) override;
};

class GameMessageHandler : public MultiplayerRoomMessageHandler
{
public:
	GameMessageHandler(std::shared_ptr<MultiplayerRoom> room);

	virtual void HandleMessage(std::shared_ptr<SocketIOSession> session, std::shared_ptr<google::protobuf::MessageLite> message) override;
};