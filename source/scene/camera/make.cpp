/*
 * make.cpp
 *
 *  Created on: 22.02.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include <boost/log/trivial.hpp>

namespace rt {
namespace scene {
namespace camera {

instance_t
make(const description_t& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make camera";

	const instance_t instance
	{
		make_point(description.origin),
		make_direction(description.direction),
		make_direction(description.up),
		normalize(cross(description.direction, description.up))
	};
	return std::move(instance);
}

}
}
}
