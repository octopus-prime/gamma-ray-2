/*
 * reader.hpp
 *
 *  Created on: 02.03.2015
 *      Author: mike_gresens
 */

#pragma once

#include "model_fwd.hpp"
#include <functional>
//#include "rply.h"
//#include <memory>

namespace rt {
namespace surface {
namespace primitive {
namespace mesh {

typedef std::function<void (vertexes_t& vertexes, faces_t& faces)> reader_t;

reader_t
open(const std::string& file);

}
}
}
}
