#include "GameLiftManager.h"

#include <aws/gamelift/server/GameLiftServerAPI.h>

void GameLiftManager::Initialize(int port)
{
	auto t = Aws::GameLift::Server::InitSDK();
}
