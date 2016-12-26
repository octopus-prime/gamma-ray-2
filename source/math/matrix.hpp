/*
 * matrix.hpp
 *
 *  Created on: 13.10.2015
 *      Author: mike_gresens
 */

#pragma once

#include "vector.hpp"
#include <array>

namespace rt {

typedef std::array<vector_t, 4> matrix_t;

constexpr matrix_t
identity
{{
	{1, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 1}
}};

inline matrix_t
transpose(const matrix_t& matrix) noexcept
{
	matrix_t result = matrix;
	_MM_TRANSPOSE4_PS(result[0], result[1], result[2], result[3]);
	return result;
}

inline matrix_t
operator+(const matrix_t& m1, const matrix_t& m2) noexcept
{
	return matrix_t
	{{
		m1[0] + m2[0],
		m1[1] + m2[1],
		m1[2] + m2[2],
		m1[3] + m2[3]
	}};
}

inline matrix_t
operator*(const matrix_t& m, const float a) noexcept
{
	return matrix_t
	{{
		m[0] * a,
		m[1] * a,
		m[2] * a,
		m[3] * a
	}};
}

inline vector_t
operator*(const matrix_t& m, const vector_t& v) noexcept
{
	return	m[0] * v[0] +
			m[1] * v[1] +
			m[2] * v[2] +
			m[3] * v[3];
}

inline vector_t
operator*(const vector_t& v, const matrix_t& m) noexcept
{
	return transpose(m) * v;
}

matrix_t
operator*(const matrix_t& m1, const matrix_t& m2) noexcept;

inline void
operator*=(matrix_t& m1, const matrix_t& m2) noexcept
{
	m1 = m1 * m2;
}

matrix_t
invert(const matrix_t& m);

}

namespace std {

ostream&
operator<<(ostream& s, const rt::matrix_t& m);

}
