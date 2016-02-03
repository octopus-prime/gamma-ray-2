/*
 * box.hpp
 *
 *  Created on: 03.03.2015
 *      Author: mgresens
 */

#pragma once

#include <geo/point.hpp>
#include <math/matrix.hpp>
#include <vector>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/range/algorithm/transform.hpp>

namespace rt {

typedef geo::model::box<point_t> box_t;
/*
inline box_t
transform(const matrix_t& transformation, const box_t& box)
{
	const matrix_t matrix = invert(transformation);
	const vector_t& min = box.min_corner();
	const vector_t& max = box.max_corner();

	geo::model::linestring<vector_t> points;
	points.emplace_back(vector_t{min[X], min[Y], min[Z]});
	points.emplace_back(vector_t{min[X], min[Y], max[Z]});
	points.emplace_back(vector_t{min[X], max[Y], min[Z]});
	points.emplace_back(vector_t{min[X], max[Y], max[Z]});
	points.emplace_back(vector_t{max[X], min[Y], min[Z]});
	points.emplace_back(vector_t{max[X], min[Y], max[Z]});
	points.emplace_back(vector_t{max[X], max[Y], min[Z]});
	points.emplace_back(vector_t{max[X], max[Y], max[Z]});

	boost::transform
	(
		points,
		points.begin(),
		[&matrix](const vector_t& point)
		{
			return matrix * point;
		}
	);

//	boost::for_each // TODO: hack :(
//	(
//		points,
//		[](vector3_t& point)
//		{
//			boost::for_each
//			(
//				point,
//				[](float& f)
//				{
//					if (f > +1e10f)
//						f = +1e10f;
//					if (f < -1e10f)
//						f = -1e10f;
//				}
//			);
//		}
//	);

	return geo::return_envelope<box_t>(points);
}
*/
}
