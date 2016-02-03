/*
 * instance.hpp
 *
 *  Created on: 24.02.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../../surface/instance.hpp"
#include "../../texture/instance.hpp"

#include "../../rendering/ray.hpp"
#include "../../rendering/hit.hpp"

#include <vector>
#include <algorithm>


/*
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/geometry.hpp>
#include "../../adapter.hpp"

BOOST_GEOMETRY_REGISTER_VECTOR_CS(cs::cartesian)

namespace rt {

namespace geo = boost::geometry;
typedef geo::model::box<vector_t> box_t;
typedef geo::model::segment<vector_t> segment_t;

}
*/
namespace rt {
namespace scene {
namespace object {

class instance_t
{
public:
	instance_t(const surface::instance_t& surface, const texture::instance_t& texture)
	:
		_surface(surface),
		_texture(texture)
//		,_box(make_point(-1, -1, -1), make_point(+1, +1, +1))
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
//		const segment_t segment(ray.origin + ray.direction * ray.min, ray.origin + ray.direction * ray.max);
//		if (!geo::intersects(_box, segment))
//			return hits;

		const auto end = _surface->hit(ray, hits);

		if (end == hits)
			return hits;

		*hits = *std::min_element(hits, end);
		hits->object = this;

		return hits + 1;
	}

	const surface::instance_t&
	surface() const noexcept
	{
		return _surface;
	}

	const texture::instance_t&
	texture() const noexcept
	{
		return _texture;
	}

private:
	surface::instance_t _surface;
	texture::instance_t _texture;
//	box_t _box;
};

typedef std::vector<instance_t> instances_t;

}
}
}
