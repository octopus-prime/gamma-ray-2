/*
 * make.cpp
 *
 *  Created on: 22.02.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include <boost/log/trivial.hpp>

namespace rt {
namespace texture {

instance_t
make(const description_t& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make texture";

	const instance_t instance
	{
		description.ambient * description.pigment,
		description.diffuse * description.pigment,
		description.specular * vector_t {1,1,1,0},
		description.shininess,
		description.reflection
	};
	return std::move(instance);
}

}
}
