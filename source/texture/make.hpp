/*
 * make.hpp
 *
 *  Created on: 22.02.2015
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "instance.hpp"

namespace rt {
namespace texture {

instance_t
make(const description_t& description);

}
}
