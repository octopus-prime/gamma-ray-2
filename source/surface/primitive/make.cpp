/*
 * make.cpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "sphere/make.hpp"
#include "plane/make.hpp"
#include "quadric/make.hpp"
#include "torus/make.hpp"
#include "sor/make.hpp"
#include "box/make.hpp"
#include "text/make.hpp"
#include "mesh/make.hpp"
#include "isosurface/make.hpp"
#include "blob/make.hpp"

namespace rt {
namespace surface {
namespace primitive {

struct visitor : boost::static_visitor<instance_t>
{
	template <typename Description>
	result_type
	operator()(const Description& description) const;
};

instance_t
make(const description_t& description)
{
	return boost::apply_visitor(visitor(), description);
}

template<>
visitor::result_type
visitor::operator()(const sphere::description_t& description) const
{
	return sphere::make(description);
}

template<>
visitor::result_type
visitor::operator()(const plane::description_t& description) const
{
	return plane::make(description);
}

template<>
visitor::result_type
visitor::operator()(const quadric::description_t& description) const
{
	return quadric::make(description);
}

template<>
visitor::result_type
visitor::operator()(const torus::description_t& description) const
{
	return torus::make(description);
}

template<>
visitor::result_type
visitor::operator()(const sor::description_t& description) const
{
	return sor::make(description);
}

template<>
visitor::result_type
visitor::operator()(const box::description_t& description) const
{
	return box::make(description);
}

template<>
visitor::result_type
visitor::operator()(const text::description_t& description) const
{
	return text::make(description);
}

template<>
visitor::result_type
visitor::operator()(const mesh::description_t& description) const
{
	return mesh::make(description);
}

template<>
visitor::result_type
visitor::operator()(const isosurface::description_t& description) const
{
	return isosurface::make(description);
}

template<>
visitor::result_type
visitor::operator()(const blob::description_t& description) const
{
	return blob::make(description);
}

}
}
}
