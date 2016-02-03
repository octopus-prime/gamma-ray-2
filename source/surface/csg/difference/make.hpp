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
namespace difference {

instance_t
make(const description_t<difference_tag>& description);

}
}
}
}
