// MultiplayerSocketServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "SocketIOManager.h"

int main()
{
    std::cout << "Hello World!\n";

	SocketIOManager manager;
	if (!manager.Initialize())
	{
		return -1;
	}

	manager.Start();

	return 0;
}