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
/*
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

//	matrix_t m
//	{{
//		{x * x * (1 - c) + c,		x * y * (1 - c) + s * z,	x * z * (1 - c) - s * y,	0},
//		{x * y * (1 - c) - s * z,	y * y * (1 - c) + c,		y * z * (1 - c) + s * x,	0},
//		{x * z * (1 - c) + s * y,	y * z * (1 - c) - s * x,	z * z * (1 - c) + c,		0},
//		{0,							0,							0,							1}
//	}};

	matrix_t m
	{{
		{x * x + (1 - x * x) * c,	x * y * (1 - c) + s * z,	x * z * (1 - c) - s * y,	0},
		{x * y * (1 - c) - s * z,	y * y + (1 - y * y) * c,	y * z * (1 - c) + s * x,	0},
		{x * z * (1 - c) + s * y,	y * z * (1 - c) - s * x,	z * z + (1 - z * z) * c,	0},
		{0,							0,							0,							1}
	}};

//	const auto f = [&](const vector_t i)
//	{
//		return normalized * dot(normalized, i) + c * cross(cross(normalized, i), normalized) + s * cross(normalized, i);
//	};
//
//	matrix_t n
//	{{
//		f(vector_t {1,0,0,0}),
//		f(vector_t {0,1,0,0}),
//		f(vector_t {0,0,1,0}),
//		{0, 0, 0, 1}
//	}};
//
//	std::cout << m << std::endl;
//	std::cout << n << std::endl;

	return m;
}
*/

matrix_t
rotation(const vector_t& vector)
{
	constexpr float f = -M_PI / 180.0;

	const auto rotateX = [&vector]() -> matrix_t
	{
		const float angle = f * vector[0];
		const float s = std::sin(angle);
		const float c = std::cos(angle);
		return matrix_t
		{{
			{1.0,	0.0,	0.0,	0.0},
			{0.0,	+c,		+s,		0.0},
			{0.0,	-s,		+c,		0.0},
			{0.0,	0.0,	0.0,	1.0}
		}};
	};

	const auto rotateY = [&vector]() -> matrix_t
	{
		const float angle = f * vector[1];
		const float s = std::sin(angle);
		const float c = std::cos(angle);
		return matrix_t
		{{
			{+c,	0.0,	-s,		0.0},
			{0.0,	1.0,	0.0,	0.0},
			{+s,	0.0,	+c,		0.0},
			{0.0,	0.0,	0.0,	1.0}
		}};
	};

	const auto rotateZ = [&vector]() -> matrix_t
	{
		const float angle = f * vector[2];
		const float s = std::sin(angle);
		const float c = std::cos(angle);
		return matrix_t
		{{
			{+c,	+s,		0.0,	0.0},
			{-s,	+c,		0.0,	0.0},
			{0.0,	0.0,	1.0,	0.0},
			{0.0,	0.0,	0.0,	1.0}
		}};
	};

	return rotateX() * rotateY() * rotateZ();
}

}
