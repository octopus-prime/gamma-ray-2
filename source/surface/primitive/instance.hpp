/*
 * instance.hpp
 *
 *  Created on: 30.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../instance.hpp"
#include "../../math/matrix.hpp"
#include <algorithm>
//#include <boost/iterator/filter_iterator.hpp>

namespace rt {
namespace surface {
namespace primitive {

template <typename Model>
class instance_impl_t
:
	public virtual instance_base_t
{
public:
	instance_impl_t(const matrix_t& transformation, Model&& model);

	virtual rendering::hits_t::iterator hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const override;
	virtual bool inside(const vector_t& point) const override;

private:
	matrix_t _scene_object;
	matrix_t _object_scene;
	Model _model;
};

template <typename Model>
instance_impl_t<Model>::instance_impl_t(const matrix_t& transformation, Model&& model)
:
	instance_base_t(),
	_scene_object(transformation),
	_object_scene(transpose(transformation)),
	_model(std::forward<Model>(model))
{
}

template <typename Model>
rendering::hits_t::iterator
instance_impl_t<Model>::hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
{
//	const auto check = [&ray](const rendering::hit_t& hit){return ray.min < hit.distance && hit.distance < ray.max;};
//	if (ray.origin[3] != 1)
//		throw std::runtime_error("origin1 " + std::to_string(ray.origin[3]));
//	if (ray.direction[3] != 0)
//		throw std::runtime_error("direction1 " + std::to_string(ray.direction[3]));
//	const rendering::ray_t r{_scene_object * ray.origin, _scene_object * ray.direction, ray.min, ray.max};
//	if (r.origin[3] != 1)
//		throw std::runtime_error("origin2 " + std::to_string(r.origin[3]));
//	if (r.direction[3] != 0)
//		throw std::runtime_error("direction2 " + std::to_string(r.direction[3]));
//	const auto end = _model.hit(r, hits);
	const auto end = _model.hit({_scene_object * ray.origin, _scene_object * ray.direction, ray.min, ray.max}, hits);
	std::for_each
	(
		hits, end,
		[this, &ray](rendering::hit_t& hit)
		{
//		if (hit.point[3] != 1)
//			throw std::runtime_error("point1 " + std::to_string(hit.point[3]));
//		if (hit.normal[3] != 0)
//			throw std::runtime_error("normal1 " + std::to_string(hit.normal[3]));
//			hit.point = _object_scene * hit.point;
			hit.point = ray.origin + ray.direction * hit.distance;
			hit.normal = _object_scene * hit.normal;
//			hit.normal = normalize(_object_scene * hit.normal);
//			if (hit.normal[3])
//				hit.normal[3] /= hit.normal[3];
			hit.normal[3] = 0;
			hit.normal = normalize(hit.normal);

//			if (hit.point[3] != 1)
//				throw std::runtime_error("point2 " + std::to_string(hit.point[3]));
//			if (hit.normal[3] != 0)
//				throw std::runtime_error("normal2 " + std::to_string(hit.normal[3]));
		}
	);
	return end;
//	return std::transform
//	(
//		boost::make_filter_iterator(check, hits, end),
//		boost::make_filter_iterator(check, end, end),
//		hits,
//		[this, &ray](const rendering::hit_t& hit)
//		{
//			return rendering::hit_t
//			{
////				ray.origin + ray.direction * hit.distance,
//				_object_scene * hit.point,
//				_object_scene * hit.normal,
//				hit.distance
//			};
//		}
//	);
}

template <typename Model>
bool
instance_impl_t<Model>::inside(const vector_t& point) const
{
	return _model.inside(_scene_object * point);
}

}
}
}
