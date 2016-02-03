/*
 * make.cpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "primitive/make.hpp"
#include "csg/make.hpp"

namespace rt {
namespace surface {

struct visitor : boost::static_visitor<instance_t>
{
	result_type
	operator()(const primitive::description_t& description) const;

	template <typename Tag>
	result_type
	operator()(const csg::description_t<Tag>& description) const;
};

instance_t
make(const description_t& description)
{
	return boost::apply_visitor(visitor(), description);
}

visitor::result_type
visitor::operator()(const primitive::description_t& description) const
{
	return primitive::make(description);
}

template <typename Tag>
visitor::result_type
visitor::operator()(const csg::description_t<Tag>& description) const
{
	return csg::make(description);
}

}
}
