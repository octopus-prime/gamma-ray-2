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

namespace rt {
namespace surface {
namespace primitive {
namespace sphere {

instance_t
make(const description_t& description)
{
	matrix_t transformation = identity;
	if (description.radius)
	{
		const float radius = *description.radius;
		transformation *= scaling(vector_t {radius, radius, radius, 0});
	}
	if (description.center)
	{
		const vector_t center = *description.center;
		transformation *= translation(center);
	}

	model_t model;
	return std::make_shared<instance_impl_t<model_t>>(transformation  * description.transformation, std::move(model));
}

}
}
}
}
