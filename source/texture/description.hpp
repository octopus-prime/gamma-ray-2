/*
 * description.hpp
 *
 *  Created on: 14.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../math/vector.hpp"

namespace rt {
namespace texture {

struct description_t
{
	vector_t pigment;
	float ambient;
	float diffuse;
	float specular;
	float shininess;
	float reflection;
};

}
}
