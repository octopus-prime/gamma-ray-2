/*
 * segment.hpp
 *
 *  Created on: 03.03.2015
 *      Author: mgresens
 */

#pragma once

#include <geo/point.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <rendering/ray.hpp>

namespace rt {

typedef geo::model::segment<point_t> segment_t;

inline segment_t
to_segment(const rendering::ray_t& ray)
{
	const vector_t v1 = ray.origin + ray.direction * ray.min;
	const vector_t v2 = ray.origin + ray.direction * ray.max;
	return segment_t(to_point(v1), to_point(v2));
}

}
