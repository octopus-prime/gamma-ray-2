/*
 * description.hpp
 *
 *  Created on: 28.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "sphere/description.hpp"
#include "plane/description.hpp"
#include "quadric/description.hpp"
#include "torus/description.hpp"
#include "sor/description.hpp"
#include <boost/variant.hpp>

namespace rt {
namespace surface {
namespace primitive {

typedef boost::variant
<
	sphere::description_t,
	plane::description_t,
	quadric::description_t,
	torus::description_t,
	sor::description_t
>
description_t;

}
}
}
