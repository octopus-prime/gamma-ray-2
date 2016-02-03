/*
 * transformation.cpp
 *
 *  Created on: 13.12.2015
 *      Author: mike_gresens
 */

#include "transformation.hpp"
#include <cmath>

namespace rt {

matrix_t
translation(const vector_t& vector)
{
	const vector_t inverted = -vector;

	const float x = inverted[0];
	const float y = inverted[1];
	const float z = inverted[2];

	return matrix_t
	{{
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{x, y, z, 1}
	}};
}

matrix_t
scaling(const vector_t& vector)
{
	const vector_t inverted = _mm_rcp_ps(vector);

	const float x = inverted[0];
	const float y = inverted[1];
	const float z = inverted[2];

	return matrix_t
	{{
		{x, 0, 0, 0},
		{0, y, 0, 0},
		{0, 0, z, 0},
		{0, 0, 0, 1}
	}};
}

matrix_t
rotation(const vector_t& vector)
{
	constexpr float factor = -M_PI / 180.0;

	const float angle = length(vector);
	const vector_t normalized = vector / angle;

	const float x = normalized[0];
	const float y = normalized[1];
	const float z = normalized[2];

	const float s = std::sin(angle * factor);
	const float c = std::cos(angle * factor);

	return matrix_t
	{{
		{x * x * (1 - c) + c,		x * y * (1 - c) + s * z,	x * z * (1 - c) - s * y,	0},
		{x * y * (1 - c) - s * z,	y * y * (1 - c) + c,		y * z * (1 - c) + s * x,	0},
		{x * z * (1 - c) + s * y,	y * z * (1 - c) - s * x,	z * z * (1 - c) + c,		0},
		{0,							0,							0,							1}
	}};
}

}
