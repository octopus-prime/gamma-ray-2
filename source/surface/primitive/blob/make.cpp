/*
 * make.cpp
 *
 *  Created on: 26.12.2016
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
namespace blob {

instance_t
make(const description_t& description)
{
	constexpr int N = 200;

	BOOST_LOG_TRIVIAL(debug) << "Make surface blob";

//	box_t box({{-3,-3,-3}}, {{+3,+3,+3}});	// todo
	const vector_t minBox {-3, -3, -3, 1};
	const vector_t maxBox {+3, +3, +3, 1};
	const vector_t delta = (maxBox - minBox) / float(N);

	const auto potential = [&description](const vector_t& point) -> float
	{
		return boost::accumulate
		(
			description.carriers,
			0.f,
			[&point](const float potential, const carrier_t& carrier)
			{
				const vector_t vector = point - carrier.first;
				const float radius2 = dot(vector, vector);
				return potential + carrier.second / radius2;
			}
		);
	};

	rtree_t rtree;

	for (size_t x = 0; x < N; ++x)
		for (size_t y = 0; y < N; ++y)
			for (size_t z = 0; z < N; ++z)
			{
				const vector_t t {float(x), float(y), float(z), 0.f};
				const vector_t min = minBox + delta * t;
				const vector_t max = min + delta;

				std::size_t count = 0;
				count += potential(vector_t{min[X], min[Y], min[Z]}) >= description.threshold;
				count += potential(vector_t{min[X], min[Y], max[Z]}) >= description.threshold;
				count += potential(vector_t{min[X], max[Y], min[Z]}) >= description.threshold;
				count += potential(vector_t{min[X], max[Y], max[Z]}) >= description.threshold;
				count += potential(vector_t{max[X], min[Y], min[Z]}) >= description.threshold;
				count += potential(vector_t{max[X], min[Y], max[Z]}) >= description.threshold;
				count += potential(vector_t{max[X], max[Y], min[Z]}) >= description.threshold;
				count += potential(vector_t{max[X], max[Y], max[Z]}) >= description.threshold;

				if (count != 0 && count != 8)
				{
					const box_t box(to_point(min), to_point(max));
					rtree.insert(std::make_pair(box, boost::none));
				}
			}

	model_t model(std::move(description.carriers), description.threshold, std::move(rtree));
	return std::make_shared<instance_impl_t<model_t>>(description.transformation, std::move(model));
}

}
}
}
}
