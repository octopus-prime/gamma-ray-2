/*
 * model.hpp
 *
 *  Created on: 10.01.2014
 *      Author: mike_gresens
 */

#pragma once

namespace rt {
namespace surface {
namespace csg {
namespace difference {

class model_t
{
public:
	model_t(const surface::instance_t& surface1, const surface::instance_t& surface2)
	:
		_surface1(surface1),
		_surface2(surface2)
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		const auto end1 = _surface1->hit(ray, hits);

		if (end1 == hits)
			return hits;

		const auto end2 = _surface2->hit(ray, end1);

		const auto hits1 = std::copy_if
		(
			hits, end1,
			hits,
			[this](const rendering::hit_t& hit)
			{
				return !_surface2->inside(hit.point);
			}
		);
		const auto hits2 = std::copy_if
		(
			end1, end2,
			hits1,
			[this](const rendering::hit_t& hit)
			{
				return _surface1->inside(hit.point);
			}
		);

		std::for_each
		(
			hits1, hits2,
			[](rendering::hit_t& hit)
			{
				hit.normal = -hit.normal;
			}
		);
		return hits2;
	}

	bool
	inside(const vector_t& point) const
	{
		return _surface1->inside(point) && !_surface2->inside(point);
	}

private:
	surface::instance_t _surface1;
	surface::instance_t _surface2;
};

}
}
}
}
