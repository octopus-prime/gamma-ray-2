/*
 * instance.hpp
 *
 *  Created on: 24.02.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../../math/vector.hpp"

namespace rt {
namespace scene {
namespace camera {

struct instance_t
{
	vector_t origin;
	vector_t direction;
	vector_t up;
	vector_t right;
};

/*
class instance_t
{
public:
	instance_t(const vector3_t& origin, const vector3_t& direction, const vector3_t& up) noexcept
	:
		_origin(origin),
		_direction(direction),
		_up(up),
		_right(normalize(_direction % _up))
	{
	}

	const vector3_t& origin() const
	{
		return _origin;
	}

	const vector3_t& direction() const
	{
		return _direction;
	}

	const vector3_t& up() const
	{
		return _up;
	}

	const vector3_t& right() const
	{
		return _right;
	}

	rendering::ray_t
	operator()(const vector2_t& point) const
	{
		const rendering::ray_t ray
		(
			_origin,
			normalize(_direction + _right * point[X] + _up * point[Y])
		);
		return std::move(ray);
	}

private:
	vector3_t _origin;
	vector3_t _direction;
	vector3_t _up;
	vector3_t _right;
};
*/
}
}
}
