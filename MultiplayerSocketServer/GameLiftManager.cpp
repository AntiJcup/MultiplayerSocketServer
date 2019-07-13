#include "GameLiftManager.h"

#ifndef _DEBUG
#include <aws/gamelift/server/GameLiftServerAPI.h>
#endif 

void GameLiftManager::Initialize(int port)
{
#ifndef _DEBUG
	auto t = Aws::GameLift::Server::InitSDK();
#endif
}
