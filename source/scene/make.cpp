/*
 * make.cpp
 *
 *  Created on: 22.02.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "camera/make.hpp"
#include "light/make.hpp"
#include "object/make.hpp"
#include <boost/range/algorithm/transform.hpp>
#include <boost/log/trivial.hpp>
#include <tbb/parallel_for_each.h>
#include <mutex>

namespace rt {
namespace scene {

instance_t
make(const description_t& description)
{
	BOOST_LOG_TRIVIAL(debug) << "Make scene";

	camera::instance_t camera = camera::make(description.camera);

	light::instances_t lights;
	boost::transform
	(
		description.lights,
		std::back_inserter(lights),
		light::make
	);

	std::mutex mutex;
	object::instances_t objects;
	tbb::parallel_for_each
	(
		description.objects.begin(), description.objects.end(),
		[&mutex, &objects](const object::description_t& description)
		{
			const object::instance_t instance = object::make(description);
			std::unique_lock<std::mutex> lock(mutex);
			objects.emplace_back(std::move(instance));
		}
	);
//	boost::transform
//	(
//		description.objects,
//		std::back_inserter(objects),
//		object::make
//	);

//	object::instances_t objects;
//	rtree_t rtree;
//	std::size_t hits = 0;
//	for (std::size_t i = 0; i < description->objects.size(); ++i)
//	{
//		const auto& d = description->objects[i];
//		const auto o = object::make(d);
//		objects.push_back(o.get<0>());
//		rtree.insert(value_t(o.get<1>(), i));
//		hits += o.get<2>();
//	}
//
//	const box_t box// TODO: puke =>
//	(
//		{{
//			geo::get<X>(rtree.bounds().min_corner()),
//			geo::get<Y>(rtree.bounds().min_corner()),
//			geo::get<Z>(rtree.bounds().min_corner())
//		}},
//		{{
//			geo::get<X>(rtree.bounds().max_corner()),
//			geo::get<Y>(rtree.bounds().max_corner()),
//			geo::get<Z>(rtree.bounds().max_corner())
//		}}
//	);
//
//	BOOST_LOG_TRIVIAL(trace) << "Make scene";
//	BOOST_LOG_TRIVIAL(trace) << "Box: min = " << box.min_corner() << ", max = " << box.max_corner() << std::endl;

	const instance_t instance
	(
		std::move(camera),
		std::move(lights),
		std::move(objects)
//		std::move(rtree)
	);

	return std::move(instance);
//	return boost::make_tuple(std::move(instance), hits);
}

}
}
