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

class basic_mesh_t
{
public:
	basic_mesh_t(vertexes_t&& vertexes, faces_t&& faces, normals_t&& normals)
	:
		_vertexes(std::forward<vertexes_t>(vertexes)),
		_faces(std::forward<faces_t>(faces)),
		_normals(std::forward<normals_t>(normals))
	{
	}

	std::size_t
	size() const
	{
		return _faces.size();
	}

	const vector_t&
	point(const std::size_t i, const std::size_t j) const
	{
		return _vertexes[_faces[i][j]];
	}

	const vector_t&
	normal(const std::size_t i, const std::size_t j) const
	{
		return _normals[_faces[i][j]];
	}

private:
	vertexes_t _vertexes;
	faces_t _faces;
	normals_t _normals;
};

typedef std::shared_ptr<basic_mesh_t> mesh_t;

class triangle_t
{
	typedef std::array<vector_t, 2> edge_t;	// Plücker's U,V

public:
	triangle_t(const mesh_t& mesh, const std::uint32_t index)
	:
		_mesh(mesh),
		_index(index),
		_normal(normalize(cross((point(1) - point(0)), (point(2) - point(0))))),
		_distance(dot(_normal, point(0) - O))
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		const edge_t edge {{ray.direction, cross(ray.direction, ray.origin - O)}};

		const bool side0 = test<1,0>(edge);
		const bool side1 = test<2,1>(edge);

		if (side0 == side1)
		{
			const bool side2 = test<0,2>(edge);
			if (side0 == side2)
			{
				const float t = (_distance - dot(_normal, ray.origin - O)) / dot(_normal, ray.direction);
				if (ray.min <= t && t <= ray.max)
				{
					const vector_t p = ray.origin + ray.direction * t;

					const float a = area(point(0), point(1), point(2));
					const float a1 = area(p, point(0), point(2)) / a;
					const float a2 = area(p, point(0), point(1)) / a;
					const float a0 = 1.f - a1 - a2;

					const vector_t n = normal(0) * a0 + normal(1) * a1 + normal(2) * a2;

					hits->distance = t;
					hits->normal = n;

					return hits + 1;
				}
			}
		}

		return hits;
	}

protected:
	template <std::size_t F, std::size_t S>
	bool
	test(const edge_t& edge) const
	{
		const edge_t temp {{point(F) - point(S), cross(point(F), point(S))}};
		return dot(edge[0], temp[1]) + dot(edge[1], temp[0]) >= 0.0f;
	}

	static float
	area(const vector_t& p0, const vector_t& p1, const vector_t& p2)
	{
		return length(cross((p1 - p0), (p2 - p0)));
	}

	const vector_t&
	point(const std::size_t index) const
	{
		return _mesh->point(_index, index);
	}

	const vector_t&
	normal(const std::size_t index) const
	{
		return _mesh->normal(_index, index);
	}

private:
	const mesh_t _mesh;
	const std::uint32_t _index;
	vector_t _normal;
	float _distance;
};

typedef std::vector<triangle_t> triangles_t;
typedef std::pair<box_t, std::uint32_t> value_t;
typedef geo::index::rtree<value_t, geo::index::quadratic<8>> rtree_t; // TODO: parse parameter

class model_t
{
public:
	model_t(triangles_t&& triangles, rtree_t&& rtree)
	:
		_triangles(std::forward<triangles_t>(triangles)),
		_rtree(std::forward<rtree_t>(rtree))
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		const auto test = [this, &ray, &hits](const value_t& value)
		{
			const triangle_t& triangle = _triangles[value.second];
			hits = triangle.hit(ray, hits);
		};

		const segment_t segment = to_segment(ray);
//		const size_t n =
		_rtree.query(geo::index::intersects(segment), boost::make_function_output_iterator(test));

//		*hits = *std::min_element(hits, hits + n); // TODO: deliver max element too ?!

		return hits;// + 1;
	}

	bool
	inside(const vector_t& point) const
	{
		return false;	// todo
	}

private:
	triangles_t _triangles;
	rtree_t _rtree;
};

}
}
}
}
