#pragma once
enum class RoomState
{
	None = 0,
	Lobby = 1,
	Load = 2,
	Ready = 3,
	Playing = 4,
	Done = 5,
};

class MultiplayerRoom
{
public:
	RoomState get_room_state() const
	{
		return room_state_;
	}

	void set_room_state(RoomState room_state)
	{
		room_state_ = room_state;
	}
private:
	RoomState room_state_;
};

