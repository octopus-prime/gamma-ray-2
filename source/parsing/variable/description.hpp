/*
 * description.hpp
 *
 *  Created on: 17.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../../scene/description.hpp"
#include <unordered_map>

namespace rt {
namespace parsing {
namespace variable {

class tag {};

typedef boost::variant
<
	vector_t,
	surface::description_t,
	texture::description_t,
	scene::camera::description_t,
	scene::light::description_t,
	scene::object::description_t,
	scene::description_t
>
description_t;

typedef std::unordered_map<std::string, description_t> descriptions_t;

}
}
}
