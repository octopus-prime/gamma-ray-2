/*
 * model.hpp
 *
 *  Created on: 04.01.2014
 *      Author: mike_gresens
 */

#pragma once

#include "model_fwd.hpp"
#include <geo/segment.hpp>
#include <geo/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/function_output_iterator.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace mesh {

typedef std::pair<box_t, std::uint32_t> value_t;
typedef geo::index::rtree<value_t, geo::index::quadratic<8>> rtree_t; // TODO: parse parameter

class model_t
{
	typedef std::array<vector_t, 2> edge_t;	// Plücker's U,V

public:
	model_t(faces_t&& faces, vertexes_t&& vertexes, normals_t&& normals, rtree_t&& rtree)
	:
		_faces(std::forward<faces_t>(faces)),
		_vertexes(std::forward<vertexes_t>(vertexes)),
		_normals(std::forward<normals_t>(normals)),
		_rtree(std::forward<rtree_t>(rtree))
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		const edge_t edge {{ray.direction, cross(ray.direction, ray.origin - O)}};

		const auto triangle = [this, &ray, &hits, &edge](const value_t& value)
		{
			const face_t& face = _faces[value.second];

			const vector_t p0 = _vertexes[face[0]];
			const vector_t p1 = _vertexes[face[1]];
			const vector_t p2 = _vertexes[face[2]];

			const bool side0 = test(edge, p1, p0);
			const bool side1 = test(edge, p2, p1);

			if (side0 == side1)
			{
				const bool side2 = test(edge, p0, p2);
				if (side0 == side2)
				{
					const vector_t normal = normalize(cross((p1 - p0), (p2 - p0)));

					const float _distance(dot(normal, p0 - O));

					const float t = (_distance - dot(normal, ray.origin - O)) / dot(normal, ray.direction);
					if (ray.min <= t && t <= ray.max)
					{
						const vector_t p = ray.origin + ray.direction * t;

						const float a = area(p0, p1, p2);
						const float a1 = area(p, p0, p2) / a;
						const float a2 = area(p, p0, p1) / a;
						const float a0 = 1.f - a1 - a2;

						const vector_t n0 = _normals[face[0]];
						const vector_t n1 = _normals[face[1]];
						const vector_t n2 = _normals[face[2]];

						const vector_t n = n0 * a0 + n1 * a1 + n2 * a2;

						hits->distance = t;
						hits->normal = n;

						++hits;
					}
				}
			}
		};

		const segment_t segment = to_segment(ray);
		_rtree.query(geo::index::intersects(segment), boost::make_function_output_iterator(triangle));

		return hits;
	}

	bool
	inside(const vector_t& point) const
	{
		return false;	// todo
	}

protected:
	static bool
	test(const edge_t& edge, const vector_t pf, const vector_t ps)
	{
		const edge_t temp {{pf - ps, cross(pf, ps)}};
		return dot(edge[0], temp[1]) + dot(edge[1], temp[0]) >= 0.0f;
	}

	static float
	area(const vector_t p0, const vector_t p1, const vector_t p2)
	{
		return length(cross((p1 - p0), (p2 - p0)));
	}

private:
	faces_t _faces;
	vertexes_t _vertexes;
	normals_t _normals;
	rtree_t _rtree;
};

}
}
}
}
