/*
 * model_fwd.hpp
 *
 *  Created on: 14.03.2016
 *      Author: mike_gresens
 */

#pragma once

#include <math/vector.hpp>
#include <functional>

namespace rt {
namespace surface {
namespace primitive {
namespace isosurface {

typedef std::function<float (vector_t)> function_t;

}
}
}
}
