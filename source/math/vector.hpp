/*
 * vector.hpp
 *
 *  Created on: 13.10.2015
 *      Author: mike_gresens
 */

#pragma once

#include <boost/predef/hardware/simd.h>

static_assert(BOOST_HW_SIMD_X86 >= BOOST_HW_SIMD_X86_SSE4_1_VERSION, "Minimum SSE4.1 required.");

#include <smmintrin.h>
#include <cmath>
#include <iostream>

namespace rt {

typedef __v4sf vector_t;
enum axis_t {X, Y, Z, W};

constexpr vector_t O {0, 0, 0, 1};

inline float
dot(const vector_t v1, const vector_t v2) noexcept
{
	const vector_t d = _mm_dp_ps(v1, v2, 0b01110001);
	return _mm_cvtss_f32(d);
}

inline float
length(const vector_t v) noexcept
{
	const vector_t d = _mm_dp_ps(v, v, 0b01110001);
	const vector_t s = _mm_sqrt_ss(d);
	return _mm_cvtss_f32(s);
}

inline vector_t
normalize(const vector_t v) noexcept
{
	const vector_t d = _mm_dp_ps(v, v, 0b01111111); //TODO
	const vector_t s = _mm_rsqrt_ps(d);
	return v * s;
}

inline vector_t
cross(const vector_t v1, const vector_t v2) noexcept
{
	const vector_t t1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1));
	const vector_t t2 = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 0, 2, 1));
	const vector_t r = _mm_sub_ps(_mm_mul_ps(v1, t2), _mm_mul_ps(t1, v2));
	return _mm_shuffle_ps(r, r, _MM_SHUFFLE(3, 0, 2, 1));
}

inline vector_t
make_point(const vector_t v) noexcept
{
	vector_t r = v;
	r[W] = 1;
	return r;
}

inline vector_t
make_direction(const vector_t v) noexcept
{
	vector_t r = v;
	r[W] = 0;
	return normalize(r);
}

}

namespace std {

ostream&
operator<<(ostream& s, const rt::vector_t v);

}
