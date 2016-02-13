/*
 * make.cpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "model.hpp"
#include "../../make.hpp"
#include "../instance.hpp"
#include <boost/log/trivial.hpp>

namespace rt {
namespace surface {
namespace csg {
namespace intersection {

instance_t
make(const description_t<intersection_tag>& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make surface intersection";

	model_t model(surface::make(description.surface1), surface::make(description.surface2));

	return std::make_shared<instance_impl_t<model_t>>(std::move(model));
}

}
}
}
}
