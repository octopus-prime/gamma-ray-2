/*
 * description.hpp
 *
 *  Created on: 13.02.2016
 *      Author: mike_gresens
 */

#pragma once

#include "../description_fwd.hpp"
#include "../../../math/vector.hpp"
#include <boost/optional.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace box {

struct description_t : base_description_t
{
	boost::optional<rt::vector_t> min;
	boost::optional<rt::vector_t> max;
};

}
}
}
}
