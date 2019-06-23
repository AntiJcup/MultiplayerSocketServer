#pragma once

#include "Point.h"

class BaseEntity
{

public:
	Point get_position()
	{
		return position_;
	}

	void set_position(Point position)
	{
		position_ = std::move(position);
	}

protected:
	Point position_;
};

