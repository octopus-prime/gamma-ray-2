/*
 * description.hpp
 *
 *  Created on: 09.02.2015
 *      Author: mgresens
 */

#pragma once

#include "../description_fwd.hpp"

namespace rt {
namespace surface {
namespace primitive {
namespace torus {

struct description_t : base_description_t
{
	float major;
	float minor;
};

}
}
}
}
