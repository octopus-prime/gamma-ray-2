/*
 * instance.hpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../instance.hpp"

namespace rt {
namespace surface {
namespace csg {

template <typename Model>
class instance_impl_t
:
	public virtual instance_base_t
{
public:
	instance_impl_t(Model&& model);

	virtual rendering::hits_t::iterator hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const override;
	virtual bool inside(const vector_t& point) const override;

private:
	Model _model;
};

template <typename Model>
instance_impl_t<Model>::instance_impl_t(Model&& model)
:
	instance_base_t(),
	_model(std::forward<Model>(model))
{
}

template <typename Model>
rendering::hits_t::iterator
instance_impl_t<Model>::hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
{
	return _model.hit(ray, hits);
}

template<typename Model>
bool
instance_impl_t<Model>::inside(const vector_t& point) const
{
	return _model.inside(point);
}

}
}
}
