/*
 * description.hpp
 *
 *  Created on: 28.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../../math/matrix.hpp"

namespace rt {
namespace surface {
namespace primitive {

struct base_description_t
{
	matrix_t transformation = identity;
};

}
}
}
