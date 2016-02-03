/*
 * instance.hpp
 *
 *  Created on: 29.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../rendering/ray.hpp"
#include "../rendering/hit.hpp"
#include <memory>
#include <vector>

namespace rt {
namespace surface {

class instance_base_t
{
protected:
	virtual ~instance_base_t() noexcept = default;

public:
	virtual rendering::hits_t::iterator hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const = 0;
	virtual bool inside(const vector_t& point) const = 0;
};

typedef std::shared_ptr<instance_base_t> instance_t;
typedef std::vector<instance_t> instances_t;

}
}
