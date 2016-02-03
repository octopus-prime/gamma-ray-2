/*
 * make.hpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../description.hpp"
#include "../../instance.hpp"

namespace rt {
namespace surface {
namespace csg {
namespace intersection {

instance_t
make(const description_t<intersection_tag>& description);

}
}
}
}
