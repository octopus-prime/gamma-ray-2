/*
 * vector.hpp
 *
 *  Created on: 13.10.2015
 *      Author: mike_gresens
 */

#pragma once

#include <boost/predef/hardware/simd.h>

static_assert(BOOST_HW_SIMD_X86 >= BOOST_HW_SIMD_X86_SSE3_VERSION, "Minimum SSSE3 required.");

#include <tmmintrin.h>

#if BOOST_HW_SIMD_X86 >= BOOST_HW_SIMD_X86_SSE4_1_VERSION
#include <smmintrin.h>
#endif

#include <iostream>
#include <cmath>

namespace rt {

typedef __v4sf vector_t;
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
dot(const vector_t x, const vector_t y)
{
#if BOOST_HW_SIMD_X86 >= BOOST_HW_SIMD_X86_SSE4_1_VERSION
	const vector_t d = _mm_dp_ps(x, y, 0b01110001);
	return d[0];
#else
	vector_t d = _mm_mul_ps(x, y);
	d = _mm_hadd_ps(d, d);
	d = _mm_hadd_ps(d, d);
	return d[0];
#endif
}

inline float
length(const vector_t v)
{
	return std::sqrt(dot(v, v));
}

inline vector_t
normalize(const vector_t v)
{
#if BOOST_HW_SIMD_X86 >= BOOST_HW_SIMD_X86_SSE4_1_VERSION
	const vector_t d = _mm_dp_ps(v, v, 0b11111111);
	const vector_t s = _mm_rsqrt_ps(d);
	return v * s;
#else
	const vector_t d = dot(v, v);
	const vector_t s = _mm_rsqrt_ss(d);
	return v * s;
#endif
}

inline vector_t
make_direction(const float x, const float y, const float z)
{
	const vector_t v = _mm_set_ps(0, z, y, x);
	return normalize(v);
}

inline vector_t
make_direction(const vector_t v)
{
	vector_t r = v;
	r[3] = 0;
	return normalize(r);
}

//vector_t
//cross(const vector_t x, const vector_t y);

//inline vector_t
//cross(const vector_t v1, const vector_t v2)
//{
//	return vector_t
//	{
//		v1[1] * v2[2] - v1[2] * v2[1],
//		v1[2] * v2[0] - v1[0] * v2[2],
//		v1[0] * v2[1] - v1[1] * v2[0],
//		0
//	};
//}

inline vector_t
cross(const vector_t v1, const vector_t v2)
{
	const vector_t t1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1));
	const vector_t t2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 0, 2, 1));
	const vector_t r = _mm_sub_ps(_mm_mul_ps(v1, t2), _mm_mul_ps(t1, v2));
	return _mm_shuffle_ps(r, r, _MM_SHUFFLE(3, 0, 2, 1));
}

}

namespace std {
ostream&
operator<<(ostream& s, const rt::vector_t& v);
}
