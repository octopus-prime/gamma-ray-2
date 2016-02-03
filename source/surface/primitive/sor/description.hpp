/*
 * description.hpp
 *
 *  Created on: 09.02.2015
 *      Author: mgresens
 */

#pragma once

#include "../description_fwd.hpp"
#include <vector>

namespace rt {
namespace surface {
namespace primitive {
namespace sor {

struct description_t : base_description_t
{
	std::vector<vector_t> points;
};

}
}
}
}
