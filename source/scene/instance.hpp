/*
 * instance.hpp
 *
 *  Created on: 13.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "camera/instance.hpp"
#include "light/instance.hpp"
#include "object/instance.hpp"

namespace rt {
namespace scene {

class instance_t
{
public:
	instance_t(camera::instance_t&& camera, light::instances_t&& lights, object::instances_t&& objects)
	:
		_camera(std::forward<camera::instance_t>(camera)),
		_lights(std::forward<light::instances_t>(lights)),
		_objects(std::forward<object::instances_t>(objects))
	{
	}

	const camera::instance_t&
	camera() const
	{
		return _camera;
	}

	const light::instances_t&
	lights() const
	{
		return _lights;
	}

	const object::instances_t&
	objects() const
	{
		return _objects;
	}

	rendering::hits_t::iterator
	all_hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		for (const auto& object : _objects)
			hits = object.hit(ray, hits);
		return hits;
	}

	bool
	any_hit(const rendering::ray_t& ray, const rendering::hits_t::iterator hits) const
	{
		for (const auto& object : _objects)
			if (object.hit(ray, hits) != hits)
				return true;
		return false;
	}

private:
	camera::instance_t _camera;
	light::instances_t _lights;
	object::instances_t _objects;
};

}
}
