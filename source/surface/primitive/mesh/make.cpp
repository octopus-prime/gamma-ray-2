/*
 * make.cpp
 *
 *  Created on: 13.02.2016
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "model.hpp"
#include "../instance.hpp"
#include "reader.hpp"
#include <tbb/parallel_for_each.h>
#include <boost/log/trivial.hpp>
#include <boost/geometry/io/wkt/wkt.hpp>
#include <math/transformation.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace mesh {

instance_t
make(const description_t& description)
{
	constexpr size_t M = 1024 * 1024;

	BOOST_LOG_TRIVIAL(debug) << "Make surface mesh";

	// Read vertexes and faces
	const reader_t read = open(description.file);

	vertexes_t vertexes;
	faces_t faces;
	read(vertexes, faces);

	BOOST_LOG_TRIVIAL(trace) << "Vertexes: " << vertexes.size();
	BOOST_LOG_TRIVIAL(trace) << "Faces: " << faces.size();

	// Calculate normals
	normals_t normals(vertexes.size(), vector_t{0, 0, 0, 0});

	tbb::parallel_for_each
	(
		faces.cbegin(), faces.cend(),
		[&vertexes, &faces, &normals](const face_t& face)
		{
			const vector_t normal = cross((vertexes[face[1]] - vertexes[face[0]]), (vertexes[face[2]] - vertexes[face[0]]));
			normals[face[0]] += normal;
			normals[face[1]] += normal;
			normals[face[2]] += normal;
		}
	);

	tbb::parallel_for_each
	(
		normals.begin(), normals.end(),
		[](vector_t& normal)
		{
			normal = normalize(normal);
		}
	);

	// Build rtree
	rtree_t rtree;

//	geo::model::ring<point_t> ring;
//	ring.resize(3);

	for (std::uint32_t index = 0; index < faces.size(); ++index)
	{
		const face_t& face = faces[index];

		const vector_t p0 = vertexes[face[0]];
		const vector_t p1 = vertexes[face[1]];
		const vector_t p2 = vertexes[face[2]];

		const vector_t min = _mm_min_ps(_mm_min_ps(p0, p1), p2);
		const vector_t max = _mm_max_ps(_mm_max_ps(p0, p1), p2);

		const box_t box(to_point(min), to_point(max));

//		for (std::size_t j = 0; j < 3; ++j)
//			ring[j] = to_point(vertexes[face[j]]);

//		const box_t box = geo::return_envelope<box_t>(ring);

		rtree.insert(std::make_pair(box, index));
	}

	size_t memory = vertexes.size() * sizeof(vector_t);
	memory += faces.size() * sizeof(face_t);
	memory += normals.size() * sizeof(vector_t);
	memory += rtree.size() * sizeof(value_t);

	BOOST_LOG_TRIVIAL(trace) << "Memory: " << memory / M << " MB";

	const point_t c = geo::return_centroid<point_t>(rtree.bounds());
	const matrix_t m = translation(-vector_t{c.get<0>(), c.get<1>(), c.get<2>(), 0});

	// Done
	model_t model(std::move(faces), std::move(vertexes), std::move(normals), std::move(rtree));
	return std::make_shared<instance_impl_t<model_t>>(description.transformation * m, std::move(model));
}

}
}
}
}
