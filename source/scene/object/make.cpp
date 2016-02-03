/*
 * make.cpp
 *
 *  Created on: 22.02.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "../../surface/make.hpp"
#include "../../texture/make.hpp"
#include <boost/log/trivial.hpp>

namespace rt {
namespace scene {
namespace object {

instance_t
make(const description_t& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make object";

	const instance_t instance
	(
		surface::make(description.surface),
		texture::make(description.texture)
	);
	return std::move(instance);
}

}
}
}
