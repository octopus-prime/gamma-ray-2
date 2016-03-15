/*
 * model.hpp
 *
 *  Created on: 14.03.2016
 *      Author: mike_gresens
 */

#pragma once

#include "model_fwd.hpp"
#include <geo/segment.hpp>
#include <geo/box.hpp>
#include <boost/math/tools/roots.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/function_output_iterator.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace isosurface {

typedef std::pair<box_t, boost::none_t> value_t;
typedef geo::index::rtree<value_t, geo::index::rstar<8>> rtree_t;

class model_t
{
public:
	model_t(function_t&& function, rtree_t&& rtree, float distance)
	:
		_function(std::forward<function_t>(function)),
		_rtree(std::forward<rtree_t>(rtree)),
		_distance(distance)
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		const auto function1 = [this, &ray](const float distance) -> float
		{
			const vector_t point = ray.origin + ray.direction * distance;
			return _function(point);
		};

		const auto function = [&ray, &function1](const float distance) -> boost::math::tuple<float, float>
		{
			constexpr float h = 1e-2f;

			return boost::math::make_tuple
			(
				function1(distance),
				(function1(distance + h) - function1(distance - h)) / (2.f * h)
			);
		};

		const auto test = [this, &ray, &hits, &function, &function1](const value_t& value)
		{
			constexpr float a = 1e-4f;

			const box_t& box = value.first;
			const point_t point = geo::return_centroid<point_t>(box);

			float distance = length((const vector_t&) point - ray.origin);

			if (ray.min > distance || distance > ray.max)
				return;

			boost::uintmax_t max = 100;
			distance = boost::math::tools::newton_raphson_iterate
			(
				function,
				distance - _distance + std::numeric_limits<float>::epsilon(),
				distance - _distance,
				distance + _distance,
				std::numeric_limits<float>::digits,
				max
			);

			if (std::abs(function1(distance)) < a)
			{
				const vector_t point = ray.origin + ray.direction * distance;

				if (geo::intersects(box, to_point(point)))
				{
					hits->distance = distance;
					hits->normal = normal(point);
					++hits;
				}
			}
		};

		const segment_t segment = to_segment(ray);
		_rtree.query(geo::index::intersects(segment), boost::make_function_output_iterator(test));

		return hits;
	}

	bool
	inside(const vector_t& point) const
	{
		return _function(point) >= 0.f;
	}

protected:
	vector_t
	normal(const vector_t point) const
	{
		constexpr float D = 1e-2;
		constexpr vector_t X {D, 0, 0};
		constexpr vector_t Y {0, D, 0};
		constexpr vector_t Z {0, 0, D};

		return vector_t
		{
			_function(point + X) - _function(point - X),
			_function(point + Y) - _function(point - Y),
			_function(point + Z) - _function(point - Z)
		};
	}

private:
	function_t _function;
	rtree_t _rtree;
	float _distance;
};

}
}
}
}
