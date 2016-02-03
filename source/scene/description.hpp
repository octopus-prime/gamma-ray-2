/*
 * description.hpp
 *
 *  Created on: 09.02.2015
 *      Author: mgresens
 */

#pragma once

#include "camera/description.hpp"
#include "light/description.hpp"
#include "object/description.hpp"

namespace rt {
namespace scene {

struct description_t
{
	camera::description_t camera;
	light::descriptions_t lights;
	object::descriptions_t objects;
};

}
}
