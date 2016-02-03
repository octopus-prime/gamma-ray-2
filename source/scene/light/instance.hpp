/*
 * instance.hpp
 *
 *  Created on: 13.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../../math/vector.hpp"
#include "../../rendering/color.hpp"
#include <vector>

namespace rt {
namespace scene {
namespace light {

struct instance_t
{
	vector_t source;
	rendering::color_t color;
};

typedef std::vector<instance_t> instances_t;

}
}
}
