/*
 * model.hpp
 *
 *  Created on: 04.01.2014
 *      Author: mike_gresens
 */

#pragma once

#include <math/spline.hpp>
#include <geo/segment.hpp>
#include <geo/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/function_output_iterator.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace sor {

typedef std::vector<polynomial5_t> derivations_t;
typedef std::pair<box_t, boost::uint32_t> value_t;
typedef geo::index::rtree<value_t, geo::index::rstar<8>> rtree_t; // TODO: parse parameter

class model_t
{
public:
	model_t(spline_t&& spline, derivations_t&& derivations, rtree_t&& rtree, const float heightBase, const float heightCap, const float radiusBase, const float radiusCap)
	:
		_spline(std::forward<spline_t>(spline)),
		_derivations(std::forward<derivations_t>(derivations)),
		_rtree(std::forward<rtree_t>(rtree)),
		_heightBase(heightBase),
		_heightCap(heightCap),
		_radiusBase(radiusBase),
		_radiusCap(radiusCap)
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		// Test base plane
		const float tB = (_heightBase - ray.origin[Y]) / ray.direction[Y];
		if (ray.min < tB && tB < ray.max)
		{
			const float uB = ray.origin[X] + tB * ray.direction[X];
			const float vB = ray.origin[Z] + tB * ray.direction[Z];
			const float rB = uB * uB + vB * vB;
			if (rB <= _radiusBase * _radiusBase)
			{
				hits->distance = tB;
				hits->normal = vector_t {0, -1, 0, 0};
				hits++;
			}
		}

		// Test cap plane
		const float tC = (_heightCap - ray.origin[Y]) / ray.direction[Y];
		if (ray.min < tC && tC < ray.max)
		{
			const float uC = ray.origin[X] + tC * ray.direction[X];
			const float vC = ray.origin[Z] + tC * ray.direction[Z];
			const float rC = uC * uC + vC * vC;
			if (rC <= _radiusCap * _radiusCap)
			{
				hits->distance = tC;
				hits->normal = vector_t {0, +1, 0, 0};
				hits++;
			}
		}

		const polynomial2_t d
		{{
			ray.origin[X] * ray.origin[X] + ray.origin[Z] * ray.origin[Z],
			2.0f * (ray.origin[X] * ray.direction[X] + ray.origin[Z] * ray.direction[Z]),
			ray.direction[X] * ray.direction[X] + ray.direction[Z] * ray.direction[Z]
		}};

		const auto hit = [this, &ray, &hits, &d](const value_t& value)
		{
			const spline_segment_t& segment = _spline[value.second];

			const float y = ray.origin[Y] - std::get<0>(segment);
			const polynomial3_t& s = std::get<2>(segment);

			const polynomial3_t p
			{{
				y * (y * (s[3] * y + s[2]) + s[1]) + s[0],
				ray.direction[Y] * (y * (3.0f * s[3] * y + 2.0f * s[2]) + s[1]),
				ray.direction[Y] * ray.direction[Y] * (3.0f * s[3] * y + s[2]),
				s[3] * ray.direction[Y] * ray.direction[Y] * ray.direction[Y]
			}};

//			const polynomial6_t q = p * p - d;
			polynomial6_t q = p * p;
			q -= d;

			std::array<float, 6> roots;
			const auto roots_begin = roots.begin();
			const auto roots_end = solve(q, roots_begin);

			std::for_each
			(
				roots_begin, roots_end,
				[this, &ray, &hits, &segment](const float t)
				{
					if (ray.min < t && t < ray.max)
					{
						const vector_t point = ray.origin + ray.direction * t;
						if (point[Y] >= std::get<0>(segment) && point[Y] <= std::get<1>(segment))
						{
							hits->distance = t;
							hits->normal = normal(point - O);
							hits++;
						}
					}
				}
			);
		};

		const segment_t segment = to_segment(ray);
		_rtree.query(geo::index::intersects(segment), boost::make_function_output_iterator(hit));

		return hits;
	}

	bool
	inside(const vector_t& point) const
	{
//		if (!geo::intersects(_rtree.bounds(), to_point(point)))
//			return false;

		if (point[Y] < _heightBase || point[Y] > _heightCap)
			return false;

		return std::hypot(point[X], point[Z]) <= evaluate(_spline, point[Y]);
	}

protected:
	vector_t
	normal(const vector_t& point) const
	{
		const spline_t::const_iterator segment = boost::lower_bound
		(
			_spline, point[Y],
			[](const spline_segment_t& segment, const float x)
			{
				return std::get<1>(segment) < x;
			}
		);

		const auto derivation = _derivations.cbegin() + std::distance(_spline.cbegin(), segment);

		return vector_t
		{
			point[X],
			-0.5f * evaluate(*derivation, point[Y] - std::get<0>(*segment)),
			point[Z]
		};
	}

private:
	spline_t _spline;
	derivations_t _derivations;
	rtree_t _rtree;
	float _heightBase;
	float _heightCap;
	float _radiusBase;
	float _radiusCap;
};

}
}
}
}
