#pragma once
#include <exception>

class MultiplayerException : public std::exception
{
public:
	MultiplayerException(const char* msg) :
		std::exception(msg)
	{

	}
};

enum class MultiplayerRoomExceptionType
{
	None = 0,
	RoomFull = 1,
};

class MultiplayerRoomException : public MultiplayerException
{
public:
	MultiplayerRoomException(const char* msg, MultiplayerRoomExceptionType t) :
		MultiplayerException(msg), type(t)
	{

	}

	MultiplayerRoomExceptionType type;
};