/*
 * model.hpp
 *
 *  Created on: 26.12.2016
 *      Author: mike_gresens
 */

#pragma once

#include "model_fwd.hpp"
#include "geo/box.hpp"
#include "geo/segment.hpp"
#include <boost/none.hpp>
#include <boost/math/tools/roots.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/function_output_iterator.hpp>
#include <boost/range/numeric.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace blob {

typedef std::pair<box_t, boost::none_t> value_t;
typedef geo::index::rtree<value_t, geo::index::rstar<8>> rtree_t;

class model_t
{
public:
	model_t(const carriers_t& carriers, const float threshold, rtree_t&& rtree)
	:
		_carriers(carriers),
		_threshold(threshold),
		_rtree(std::forward<rtree_t>(rtree))
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		const auto function1 = [this, &ray](const float distance) -> float
		{
			const vector_t point = ray.origin + ray.direction * distance;
			return potential(point) - _threshold;
		};

		const auto function = [this, &ray, &function1](const float distance) -> boost::math::tuple<float, float>
		{
			constexpr float h = 1e-2f;
			return boost::math::make_tuple
			(
				function1(distance),
				(function1(distance + h) - function1(distance - h)) / (2 * h)
			);
		};

		const auto test = [this, &ray, &hits, &function, &function1](const value_t& value)
		{
			const box_t& box = value.first;
			vector_t point;
			geo::centroid(box, to_point(point));
			const float l = length(point - ray.origin);

			if (l < ray.min || l > ray.max)
				return false;

			boost::uintmax_t max = 100;
			const float t = boost::math::tools::newton_raphson_iterate
			(
				function,
				l - 0.1f + std::numeric_limits<float>::epsilon(),
				l - 0.1f,
				l + 0.1f,
				std::numeric_limits<float>::digits,
				max
			);

			const float f = std::abs(function1(t));

			if (f < 1e-4f)
			{
				const vector_t point = ray.origin + ray.direction * t;
				*hits++ = {point, normal(point), t};
			}

			return false;
		};

		const segment_t segment = to_segment(ray);
		_rtree.query(geo::index::intersects(segment), boost::make_function_output_iterator(test));

		return hits;
	}

	bool
	inside(const vector_t point) const
	{
		return potential(point) >= _threshold;
	}

protected:
	float
	potential(const vector_t point) const
	{
		return boost::accumulate
		(
			_carriers,
			0.f,
			[&point](const float potential, const carrier_t& carrier)
			{
				const vector_t vector = point - carrier.first;
				const float radius2 = dot(vector, vector);
				return potential + carrier.second / radius2;
			}
		);
	}

	vector_t
	normal(const vector_t& point) const
	{
		return boost::accumulate
		(
			_carriers,
			vector_t {0, 0, 0, 0},
			[&point](const vector_t normal, const carrier_t& carrier)
			{
				const vector_t vector = point - carrier.first;
				const float radius2 = dot(vector, vector);
				return normal + (carrier.second / (radius2 * radius2)) * vector;
			}
		);
	}

private:
	carriers_t _carriers;
	float _threshold;
	rtree_t _rtree;
};

}
}
}
}
