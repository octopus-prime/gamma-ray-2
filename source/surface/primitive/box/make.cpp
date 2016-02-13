/*
 * make.cpp
 *
 *  Created on: 13.02.2016
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "model.hpp"
#include "../instance.hpp"
#include <boost/log/trivial.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace box {

constexpr std::array<std::array<vector_t, 3>, 2> model_t::NORMALS;

instance_t
make(const description_t& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make surface box";

	const vector_t min = description.min.get_value_or(vector_t {-1, -1, -1, 1});
	const vector_t max = description.max.get_value_or(vector_t {+1, +1, +1, 1});

	model_t model(min, max);
	return std::make_shared<instance_impl_t<model_t>>(description.transformation, std::move(model));
}

}
}
}
}
