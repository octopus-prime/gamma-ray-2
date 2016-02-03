/*
 * description.hpp
 *
 *  Created on: 09.02.2015
 *      Author: mgresens
 */

#pragma once

#include "../../math/vector.hpp"

namespace rt {
namespace scene {
namespace camera {

struct description_t
{
	vector_t origin;
	vector_t direction;
	vector_t up = {0, 1, 0, 0};
};

}
}
}
