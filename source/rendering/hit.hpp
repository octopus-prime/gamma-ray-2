/*
 * hit.hpp
 *
 *  Created on: 15.10.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../math/vector.hpp"
#include <vector>

namespace rt {

namespace scene {
namespace object {

class instance_t;

}
}

namespace rendering {

struct hit_t
{
	vector_t point;
	vector_t normal;
	float distance;
	const scene::object::instance_t* object;
};

typedef std::vector<hit_t> hits_t;

inline bool
operator<(const hit_t& h1, const hit_t& h2)
{
	return h1.distance < h2.distance;
}

}
}
