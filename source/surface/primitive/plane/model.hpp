/*
 * model.hpp
 *
 *  Created on: 30.11.2015
 *      Author: mike_gresens
 */

#pragma once

namespace rt {
namespace surface {
namespace primitive {
namespace plane {

class model_t
{
public:
	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		const float a = dot(N, ray.direction);
		if (a == 0)
			return hits;
		const float b = dot(N, ray.origin - O);
		const float t = -b / a;
		if (ray.min < t && t < ray.max)
		{
			const vector_t p = ray.origin + ray.direction * t;
			*hits++ = {p, N, t};
		}
		return hits;
	}

	bool
	inside(const vector_t& point) const
	{
		return false;
	}

private:
	static constexpr vector_t N = {0, 1, 0, 0};
//	static constexpr float D = -1;
};

}
}
}
}
