/*
 * make.cpp
 *
 *  Created on: 14.03.2016
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "model.hpp"
#include "../instance.hpp"
#include <boost/log/trivial.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace isosurface {

instance_t
make(const description_t& description)
{
	constexpr int N = 200;

	BOOST_LOG_TRIVIAL(debug) << "Make surface isosurface";

	const vector_t minBox = description.min.get_value_or(vector_t {-1, -1, -1, 1});
	const vector_t maxBox = description.max.get_value_or(vector_t {+1, +1, +1, 1});
	const vector_t delta = (maxBox - minBox) / float(N);

	function_t function = description.function;
	rtree_t rtree;

	for (size_t x = 0; x < N; ++x)
		for (size_t y = 0; y < N; ++y)
			for (size_t z = 0; z < N; ++z)
			{
				const vector_t t {float(x), float(y), float(z), 0.f};
				const vector_t min = minBox + delta * t;
				const vector_t max = min + delta;

				std::size_t count = 0;
				count += function(vector_t{min[X], min[Y], min[Z]}) >= 0;
				count += function(vector_t{min[X], min[Y], max[Z]}) >= 0;
				count += function(vector_t{min[X], max[Y], min[Z]}) >= 0;
				count += function(vector_t{min[X], max[Y], max[Z]}) >= 0;
				count += function(vector_t{max[X], min[Y], min[Z]}) >= 0;
				count += function(vector_t{max[X], min[Y], max[Z]}) >= 0;
				count += function(vector_t{max[X], max[Y], min[Z]}) >= 0;
				count += function(vector_t{max[X], max[Y], max[Z]}) >= 0;

				if (count != 0 && count != 8)
				{
					const box_t box(to_point(min), to_point(max));
					rtree.insert(std::make_pair(box, boost::none));
				}
			}

	model_t model(std::move(function), std::move(rtree), 0.5f * length(delta));
	return std::make_shared<instance_impl_t<model_t>>(description.transformation, std::move(model));
}

}
}
}
}
