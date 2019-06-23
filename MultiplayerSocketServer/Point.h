#pragma once

#include <boost/geometry/geometry.hpp>

class Point : boost::geometry::model::point<float, 2, boost::geometry::cs::cartesian>
{
public:

	float get_x()
	{
		return get<0>();
	}

	float get_y()
	{
		return get<1>();
	}

	void set_x(float x)
	{
		set<0>(x);
	}

	void set_y(float y)
	{
		set<0>(y);
	}
};