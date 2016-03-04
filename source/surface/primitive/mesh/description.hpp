/*
 * description.hpp
 *
 *  Created on: 13.02.2016
 *      Author: mike_gresens
 */

#pragma once

#include "../description_fwd.hpp"
#include <string>

namespace rt {
namespace surface {
namespace primitive {
namespace mesh {

struct description_t : base_description_t
{
	std::string file;
};

}
}
}
}
