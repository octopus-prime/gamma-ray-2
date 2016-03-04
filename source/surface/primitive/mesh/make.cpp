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

namespace rt {
namespace surface {
namespace primitive {
namespace mesh {

instance_t
make(const description_t& description)
{
	std::cout << sizeof(face_t) << std::endl;

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

	geo::model::ring<point_t> ring;
	ring.resize(3);

	for (std::uint32_t i = 0; i < faces.size(); ++i)
	{
		const face_t& face = faces[i];

		for (std::size_t j = 0; j < 3; ++j)
			ring[j] = to_point(vertexes[face[j]]);

		rtree.insert(value_t(geo::return_envelope<box_t>(ring), i));
	}

	// Done
	model_t model(std::move(faces), std::move(vertexes), std::move(normals), std::move(rtree));
	return std::make_shared<instance_impl_t<model_t>>(description.transformation, std::move(model));
}

}
}
}
}
