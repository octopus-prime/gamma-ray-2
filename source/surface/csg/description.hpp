/*
 * description.hpp
 *
 *  Created on: 28.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../description_fwd.hpp"
#include <boost/fusion/include/define_struct.hpp>

namespace rt {
namespace surface {
namespace csg {

template <typename Tag>
struct description_t
{
	surface::description_t surface1;
	surface::description_t surface2;
};

}
}
}
