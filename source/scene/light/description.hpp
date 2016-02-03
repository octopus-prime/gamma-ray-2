/*
 * description.hpp
 *
 *  Created on: 09.02.2015
 *      Author: mgresens
 */

#pragma once

#include "../../math/vector.hpp"
#include <vector>

namespace rt {
namespace scene {
namespace light {

struct description_t
{
	vector_t origin;
	vector_t color;
};

typedef std::vector<description_t> descriptions_t;

}
}
}
