/*
 * transformation.hpp
 *
 *  Created on: 13.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "vector.hpp"
#include "matrix.hpp"

namespace rt {

matrix_t
translation(const vector_t& vector);

matrix_t
scaling(const vector_t& vector);

matrix_t
rotation(const vector_t& vector);

}
