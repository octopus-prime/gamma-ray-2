/*
 * make.cpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#include "make.hpp"
#include "model.hpp"
#include "../../make.hpp"
#include "../instance.hpp"

namespace rt {
namespace surface {
namespace csg {
namespace union_ {

instance_t
make(const description_t<union_tag>& description)
{
	model_t model(surface::make(description.surface1), surface::make(description.surface2));

	return std::make_shared<instance_impl_t<model_t>>(std::move(model));
}

}
}
}
}
