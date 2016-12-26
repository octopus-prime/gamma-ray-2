/*
 * description.hpp
 *
 *  Created on: 26.12.2016
 *      Author: mike_gresens
 */

#pragma once

#include "../description_fwd.hpp"
#include "model_fwd.hpp"

namespace rt {
namespace surface {
namespace primitive {
namespace blob {

struct description_t : base_description_t
{
	carriers_t carriers;
	float threshold;
};

}
}
}
}
