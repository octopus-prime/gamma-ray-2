/*
 * description.hpp
 *
 *  Created on: 14.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../rendering/color.hpp"

namespace rt {
namespace texture {

struct instance_t
{
	rendering::color_t ambient;
	rendering::color_t diffuse;
	rendering::color_t specular;
	float shininess;
	float reflection;
};

}
}
