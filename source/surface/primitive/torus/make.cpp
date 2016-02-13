/*
 * make.cpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "model.hpp"
#include "../../../math/transformation.hpp"
#include "../instance.hpp"
#include <boost/log/trivial.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace torus {

instance_t
make(const description_t& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make surface torus";

	const float max = description.major + description.minor;
	box_t box(point_t(-max, -max, -description.minor), point_t(+max, +max, +description.minor));
	model_t model(description.major, description.minor, std::move(box));
	return std::make_shared<instance_impl_t<model_t>>(description.transformation, std::move(model));
}

}
}
}
}
