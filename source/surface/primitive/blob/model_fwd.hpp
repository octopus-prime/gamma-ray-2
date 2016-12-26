/*
 * model_fwd.hpp
 *
 *  Created on: 26.12.2016
 *      Author: mike_gresens
 */

#pragma once

#include <math/vector.hpp>
#include <vector>

namespace rt {
namespace surface {
namespace primitive {
namespace blob {

typedef std::pair<vector_t, float> carrier_t;
typedef std::vector<carrier_t> carriers_t;

}
}
}
}
