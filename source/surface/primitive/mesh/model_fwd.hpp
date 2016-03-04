/*
 * model_fwd.hpp
 *
 *  Created on: 04.01.2014
 *      Author: mike_gresens
 */

#pragma once

#include <math/vector.hpp>
#include <cstdint>
#include <vector>
#include <array>

namespace rt {
namespace surface {
namespace primitive {
namespace mesh {

typedef std::vector<vector_t> vertexes_t;
typedef std::vector<vector_t> normals_t;
typedef std::array<std::uint32_t, 3> face_t;
typedef std::vector<face_t> faces_t;

}
}
}
}
