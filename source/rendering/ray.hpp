/*
 * ray.hpp
 *
 *  Created on: 15.10.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../math/vector.hpp"

namespace rt {
namespace rendering {

struct ray_t
{
	vector_t origin;
	vector_t direction;
	float min;
	float max;
};

}
}
