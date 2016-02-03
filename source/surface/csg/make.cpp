/*
 * make.cpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "union/make.hpp"
#include "difference/make.hpp"
#include "intersection/make.hpp"

namespace rt {
namespace surface {
namespace csg {

template <>
instance_t
make(const description_t<union_tag>& description)
{
	return union_::make(description);
}

template <>
instance_t
make(const description_t<difference_tag>& description)
{
	return difference::make(description);
}

template <>
instance_t
make(const description_t<intersection_tag>& description)
{
	return intersection::make(description);
}

}
}
}
