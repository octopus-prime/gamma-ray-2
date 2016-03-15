/*
 * description.hpp
 *
 *  Created on: 14.03.2016
 *      Author: mike_gresens
 */

#pragma once

#include "../description_fwd.hpp"
#include "model_fwd.hpp"
#include <boost/optional.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace isosurface {

struct description_t : base_description_t
{
	function_t function;
	boost::optional<rt::vector_t> min;
	boost::optional<rt::vector_t> max;
};

}
}
}
}
