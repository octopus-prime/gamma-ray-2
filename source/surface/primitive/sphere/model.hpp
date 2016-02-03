/*
 * model.hpp
 *
 *  Created on: 30.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include <cmath>

namespace rt {
namespace surface {
namespace primitive {
namespace sphere {

class model_t
{
public:
	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		const vector_t o = ray.origin - O;

		const float c = dot(o, o) - 1;
		const float b = dot(o, ray.direction);
		const float a = dot(ray.direction, ray.direction);

		const float q = c / a;
		const float p = b / a;

		const float D = p * p - q;
		if (D < 0)
			return hits;

		const float s = std::sqrt(D);

		const float t1 = -p - s;
		if (ray.min < t1 && t1 < ray.max)
		{
			const vector_t p1 = ray.origin + ray.direction * t1;
			*hits++ = {p1, p1 - O, t1};
		}

		const float t2 = -p + s;
		if (ray.min < t2 && t2 < ray.max)
		{
			const vector_t p2 = ray.origin + ray.direction * t2;
			*hits++ = {p2, p2 - O, t2};
		}

		return hits;
	}

	bool
	inside(const vector_t& point) const
	{
		const vector_t o = point - O;
		return dot(o, o) < 1;
	}
};

}
}
}
}
