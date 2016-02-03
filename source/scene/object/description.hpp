/*
 * description.hpp
 *
 *  Created on: 09.02.2015
 *      Author: mgresens
 */

#pragma once

#include "../../surface/description.hpp"
#include "../../texture/description.hpp"
#include <vector>

namespace rt {
namespace scene {
namespace object {

struct description_t
{
	surface::description_t surface;
	texture::description_t texture;
};

typedef std::vector<description_t> descriptions_t;

}
}
}
