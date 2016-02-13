/*
 * make.cpp
 *
 *  Created on: 22.02.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "model.hpp"
#include <math/transformation.hpp>
#include "../instance.hpp"
#include <boost/log/trivial.hpp>

namespace rt {
namespace surface {
namespace primitive {
namespace sor {

surface::instance_t
make(const description_t& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make surface sor";

	const auto& points = description.points;
	spline_t spline = make_spline(points.begin(), points.end());
	derivations_t derivations;
	rtree_t rtree;

	for (std::size_t i = 0; i < spline.size(); ++i)
	{
		const spline_segment_t& segment = spline[i];
		const polynomial5_t derivation = differentiate(std::get<2>(segment) * std::get<2>(segment));

		const float delta = std::get<1>(segment) - std::get<0>(segment);
		float max = std::max
		(
			evaluate(std::get<2>(segment), 0.0f),
			evaluate(std::get<2>(segment), delta)
		);

		std::array<float, 5> roots;
		const auto end = solve(derivation, roots.begin());
		for (auto root = roots.begin(); root != end; ++root)
			if (*root >= 0.0f && *root <= delta)
				max = std::max
				({
					max,
					evaluate(std::get<2>(segment), *root)
				});

		const box_t box
		(
			point_t(-max, std::get<0>(segment), -max),
			point_t(+max, std::get<1>(segment), +max)
		);

		derivations.emplace_back(derivation);
		rtree.insert(value_t(box, i));
	}
/*
	const box_t box = transform
	(
		description.transformation,
		box_t// TODO: puke =>
		(
			vector_t {
				geo::get<X>(rtree.bounds().min_corner()),
				geo::get<Y>(rtree.bounds().min_corner()),
				geo::get<Z>(rtree.bounds().min_corner())
			},
			vector_t {
				geo::get<X>(rtree.bounds().max_corner()),
				geo::get<Y>(rtree.bounds().max_corner()),
				geo::get<Z>(rtree.bounds().max_corner())
			}
		)
	);

	BOOST_LOG_TRIVIAL(trace) << "Box: min = " << box.min_corner() << ", max = " << box.max_corner() << std::endl;

	const box_t box// TODO: puke =>
	(
		vector_t {
			geo::get<X>(rtree.bounds().min_corner()),
			geo::get<Y>(rtree.bounds().min_corner()),
			geo::get<Z>(rtree.bounds().min_corner())
		},
		vector_t {
			geo::get<X>(rtree.bounds().max_corner()),
			geo::get<Y>(rtree.bounds().max_corner()),
			geo::get<Z>(rtree.bounds().max_corner())
		}
	);

	BOOST_LOG_TRIVIAL(trace) << "Box: min = " << box.min_corner() << ", max = " << box.max_corner() << std::endl;
*/
	model_t model
	(
		std::move(spline),
		std::move(derivations),
		std::move(rtree),
		points.front()[X],
		points.back()[X],
		points.front()[Y],
		points.back()[Y]
//		box
	);

	return std::make_shared<instance_impl_t<model_t>>(description.transformation, std::move(model));

//	return boost::make_tuple
//	(
//		primitive::make<model>
//		(
//			description.transformation,
//			std::move(spline),
//			std::move(derivations),
//			std::move(rtree),
//			points.front()[X],
//			points.back()[X],
//			points.front()[Y],
//			points.back()[Y]
//		),
//		box,
//		3 * spline.size() + 2
//	);
}

}
}
}
}
