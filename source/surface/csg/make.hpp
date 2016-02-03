/*
 * make.hpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "../instance.hpp"

namespace rt {
namespace surface {
namespace csg {

template <typename Tag>
instance_t
make(const description_t<Tag>& description);

}
}
}
