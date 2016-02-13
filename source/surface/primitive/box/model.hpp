/*
 * model.hpp
 *
 *  Created on: 13.02.20016
 *      Author: mike_gresens
 */

#pragma once

namespace rt {
namespace surface {
namespace primitive {
namespace box {

class model_t
{
public:
	model_t(const vector_t min, const vector_t max)
	:
		_points {{min, max}}
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		hits[0].distance = ray.min;
		hits[1].distance = ray.max;
		return hits + 2 * (test<X>(ray, hits) && test<Y>(ray, hits) && test<Z>(ray, hits));
	}

	bool
	inside(const vector_t& point) const
	{
		return test<X>(point) && test<Y>(point) && test<Z>(point);
	}

protected:
	template <size_t N>
	bool
	test(const vector_t& point) const
	{
		return point[N] >= _points[0][N] && point[N] <= _points[1][N];
	}

	template <size_t N>
	bool
	test(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		const bool sign = std::signbit(ray.direction[N]);
		const float min = (_points[ sign][N] - ray.origin[N]) / ray.direction[N];
		const float max = (_points[!sign][N] - ray.origin[N]) / ray.direction[N];

		if (hits[0].distance > max || min > hits[1].distance)
			return false;

		if (min > hits[0].distance)
		{
			hits[0].distance = min;
			hits[0].normal = NORMALS[sign][N];
		}
		if (max < hits[1].distance)
		{
			hits[1].distance = max;
			hits[1].normal = NORMALS[!sign][N];
		}

		return true;
	}

private:
	std::array<vector_t, 2> _points;

	static constexpr std::array<std::array<vector_t, 3>, 2> NORMALS
	{{
		{{
			{-1,0,0,0}, {0,-1,0,0}, {0,0,-1,0}
		}},
		{{
			{+1,0,0,0}, {0,+1,0,0}, {0,0,+1,0}
		}}
	}};
};

}
}
}
}
