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
namespace plane {

instance_t
make(const description_t& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make surface plane";

	matrix_t transformation = identity;
	if (description.distance)
	{
		const float distance = *description.distance;

		static const vector_t normal = make_direction(0, 1, 0);
		transformation *= translation(normal * distance);
	}

	model_t model;
	return std::make_shared<instance_impl_t<model_t>>(transformation * description.transformation, std::move(model));
}

}
}
}
}
