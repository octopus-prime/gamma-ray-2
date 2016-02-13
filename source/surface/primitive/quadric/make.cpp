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
namespace quadric {

instance_t
make(const description_t& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make surface quadric";

//	matrix_t transformation = identity;
//	if (description.radius)
//	{
//		const float radius = *description.radius;
//		transformation *= scaling(vector_t {radius, radius, radius, 0});
//	}
//	if (description.center)
//	{
//		const vector_t center = *description.center;
//		transformation *= translation(center);
//	}

	matrix_t matrix
	{{
		{2 * description.square[0],	description.linear[0],		description.linear[1],		description.absolute[0]},
		{description.linear[0],		2 * description.square[1],	description.linear[2], 		description.absolute[1]},
		{description.linear[1],		description.linear[2],		2 * description.square[2],	description.absolute[2]},
		{description.absolute[0],		description.absolute[1],	description.absolute[2],	2 * description.constant}
	}};

	model_t model(std::move(matrix * 0.5f));
	return std::make_shared<instance_impl_t<model_t>>(description.transformation, std::move(model));
}

}
}
}
}
