/*
 * description.hpp
 *
 *  Created on: 17.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../description_fwd.hpp"
#include "../../../math/vector.hpp"
#include <boost/optional.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace plane {

struct description_t : base_description_t
{
	boost::optional<rt::vector_t> normal;
	boost::optional<float> distance;
};

}
}
}
}
