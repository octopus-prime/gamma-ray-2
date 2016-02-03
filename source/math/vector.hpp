/*
 * vector.hpp
 *
 *  Created on: 13.10.2015
 *      Author: mike_gresens
 */

#pragma once

#include <smmintrin.h>
#include <iostream>
#include <cmath>

namespace rt {

typedef __m128 vector_t;
enum axis_t {X, Y, Z, W};

constexpr vector_t O {0, 0, 0, 1};

inline vector_t
make_point(const float x, const float y, const float z)
{
	return _mm_set_ps(1, z, y, x);
}

inline vector_t
make_point(const vector_t v)
{
	vector_t r = v;
	r[3] = 1;
	return r;
}

inline float
length(const vector_t v)
{
//	const vector_t d = _mm_dp_ps(v, v, 0b11111111);
//	const vector_t s = _mm_sqrt_ps(d);
//	return s[0];
	const vector_t d = _mm_dp_ps(v, v, 0b01110001);
	return std::sqrt(d[0]);
}

inline vector_t
normalize(const vector_t v)
{
	const vector_t d = _mm_dp_ps(v, v, 0b11111111);
	const vector_t s = _mm_sqrt_ps(d);
	return v / s;
}

inline vector_t
make_direction(const float x, const float y, const float z)
{
	const vector_t v = _mm_set_ps(0, z, y, x);
	const vector_t d = _mm_dp_ps(v, v, 0b11111111);
	const vector_t s = _mm_sqrt_ps(d);
	return v / s;
}

inline vector_t
make_direction(const vector_t v)
{
	vector_t r = v;
	r[3] = 0;
	return normalize(r);
}

inline float
dot(const vector_t x, const vector_t y)
{
	const vector_t d = _mm_dp_ps(x, y, 0b01110001);
	return d[0];
}

vector_t
cross(const vector_t x, const vector_t y);

}

namespace std {
ostream&
operator<<(ostream& s, const rt::vector_t& v);
}
