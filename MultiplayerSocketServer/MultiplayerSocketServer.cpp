// MultiplayerSocketServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "SocketIOManager.h"
#include "MultiplayerSessionManager.h"

int main()
{
	std::cout << "Hello World!\n";

	auto address = boost::beast::net::ip::make_address("127.0.0.1");
	USHORT port = 9999;

	SocketIOManager manager(1);
	auto session_manager = std::make_shared<MultiplayerSessionManager>(manager.GetIOContext());
	session_manager->Initialize();
	if (!manager.Initialize(std::make_shared<SocketIOListener>(manager.GetIOContext(),
		boost::asio::ip::tcp::endpoint{ address, port },
		session_manager)))
	{
		return -1;
	}

	manager.Start();

	return 0;
}