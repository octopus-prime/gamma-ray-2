/*
 * color.hpp
 *
 *  Created on: 01.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include <smmintrin.h>

namespace rt {
namespace rendering {

typedef __m128 color_t;

inline color_t
make_color(const float c)
{
	return _mm_set1_ps(c);
}

inline color_t
make_color(const float r, const float g, const float b)
{
	return _mm_set_ps(0, b, g, r);
}

}
}
