/*
 * description_fwd.hpp
 *
 *  Created on: 28.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "primitive/description.hpp"
#include "csg/description_fwd.hpp"
#include <boost/variant.hpp>

namespace rt {
namespace surface {

typedef boost::variant
<
	primitive::description_t,
	boost::recursive_wrapper<csg::description_t<csg::union_tag>>,
	boost::recursive_wrapper<csg::description_t<csg::difference_tag>>,
	boost::recursive_wrapper<csg::description_t<csg::intersection_tag>>
>
description_t;

}
}
