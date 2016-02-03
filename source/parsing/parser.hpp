/*
 * parser.hpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include <string>
#include "../scene/description.hpp"

namespace rt {
namespace parsing {

scene::description_t
parse(const std::string& file);

}
}
