/*
 * model.hpp
 *
 *  Created on: 04.01.2014
 *      Author: mike_gresens
 */

#pragma once

#include "../../../math/polynomial.hpp"
#include "../../../math/vector.hpp"
#include <algorithm>
#include <geo/segment.hpp>
#include <geo/box.hpp>
//#include <polynomial/polynomial.hpp>
//#include <boost/math/tools/roots.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace torus {

class model_t
{
public:
	model_t(const float major, const float minor, box_t&& box)
	:
		_major(major),
		_minor(minor),
		_box(box)
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		const segment_t segment = to_segment(ray);
		if (!geo::intersects(_box, segment))
			return hits;

		const vector_t o = ray.origin - O;

		const float a = dot(ray.direction, ray.direction);
		const float b = dot(o, ray.direction) * 2.0f;
		const float c = dot(o, o) - _minor * _minor - _major * _major;
		const float R = 4.0f * _major * _major;

		const polynomial4_t polynomial
//		const std::array<double,5> polynomial
		{{
			c * c + R * (ray.origin[Z] * ray.origin[Z] - _minor * _minor),
			2.0f * (b * c + R * ray.origin[Z] * ray.direction[Z]),
			b * b + 2.0f * a * c + R * ray.direction[Z] * ray.direction[Z],
			2.0f * a * b,
			a * a
		}};

		std::array<float, 4> roots;
//		std::array<double, 4> roots;
		const auto roots_begin = roots.begin();
		const auto roots_end = solve(polynomial, roots_begin);
//		const auto roots_end = polynomial::solver<5,polynomial::real_tag>()(polynomial.begin(), roots_begin);

//		std::array<float, 4> roots2;
//		const auto roots2_begin = roots2.begin();
//		const auto roots2_end = solve(polynomial, roots2_begin);
//
//		if (std::distance(roots_begin, roots_end) != std::distance(roots2_begin, roots2_end))
//			throw std::runtime_error("Not equal");

//		std::transform
//		(
//			roots_begin, roots_end,
//			roots2_begin,// roots2_end,
//			roots_begin,
//			[&](const float r1, const float r2)
//			{
//				if (std::abs(evaluate(polynomial, r2)) < 0.1f* std::abs(evaluate(polynomial, r1)))
//					throw std::runtime_error("GSL better");
//				return r1;
//			}
//		);

//		if (roots_begin != roots_end)
//		{
//
//			const polynomial3_t d = differentiate(polynomial);
//
//			const auto f = [&](const float x)
//			{
//				return boost::math::make_tuple
//				(
//					evaluate(polynomial, x),
//					evaluate(d, x)
//				);
//			};

		std::for_each
		(
			roots_begin, roots_end,
//			[this, &ray, &hits, &f, &polynomial](float t)
			[this, &ray, &hits](const float t)
			{
//			float y = std::abs(evaluate(polynomial, t));
//
//			if (y > 1e-4f)
//			{
//				boost::uintmax_t max = 10;
//				t = boost::math::tools::newton_raphson_iterate
//				(
//					f,
//					t,
//					t - 0.5f,
//					t + 0.5f,
//					std::numeric_limits<float>::digits,
//					max
//				);
//
//				y = std::abs(evaluate(polynomial, t));
//			}


//			if (y < 1e-4f)
//			{
				if (ray.min < t && t < ray.max)
				{
					const vector_t p = ray.origin + ray.direction * t;
					*hits++ = {p, normal(p - O), t};
				}
//			}
			}
		);
//		}

		return hits;
	}

	bool
	inside(const vector_t& point) const
	{
		const float delta = std::hypot(point[X], point[Y]) - _major;
		return point[Z] * point[Z] + delta * delta < _minor * _minor;
	}

protected:
	vector_t
	normal(const vector_t& point) const
	{
		const float length = 4.0f * (dot(point, point) - _minor * _minor - _major * _major);
		const vector_t vector {length, length, length + 8.0f * _major * _major, 0};
		return //normalize(
				point * vector
				//)
				;
	}

private:
	float _major;
	float _minor;
	box_t _box;
};

}
}
}
}
