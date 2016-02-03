/*
 * polynomial.hpp
 *
 *  Created on: 08.01.2015
 *      Author: mike_gresens
 */

#pragma once

#include <array>
#include <cmath>
#include <limits>
#include <algorithm>
#include <complex>
#include <gsl/gsl_poly.h>
#include "solve.hpp"

namespace rt {

typedef float coeff_t;

template <std::size_t N>
using basic_polynomial_t = std::array<coeff_t, N>;

typedef basic_polynomial_t<2> polynomial1_t;
typedef basic_polynomial_t<3> polynomial2_t;
typedef basic_polynomial_t<4> polynomial3_t;
typedef basic_polynomial_t<5> polynomial4_t;
typedef basic_polynomial_t<6> polynomial5_t;
typedef basic_polynomial_t<7> polynomial6_t;
typedef basic_polynomial_t<8> polynomial7_t;
typedef basic_polynomial_t<9> polynomial8_t;

template <std::size_t M, std::size_t N>
inline basic_polynomial_t<M>&
operator-=(basic_polynomial_t<M>& polynomial1, const basic_polynomial_t<N>& polynomial2)
{
	static_assert(M >= N, "M >= N");
	std::transform
	(
		polynomial2.begin(), polynomial2.end(),
		polynomial1.begin(),
		polynomial1.begin(),
		std::bind(std::minus<coeff_t>(), std::placeholders::_2, std::placeholders::_1)
	);
	return polynomial1;
}

template <std::size_t M, std::size_t N>
inline auto
operator*(const basic_polynomial_t<M>& polynomial1, const basic_polynomial_t<N>& polynomial2)
{
	basic_polynomial_t<M+N-1> result;
	std::fill(result.begin(), result.end(), 0.0);
	for (std::size_t i = 0; i < polynomial1.size(); ++i)
		for (std::size_t j = 0; j < polynomial2.size(); ++j)
			result[i + j] += polynomial1[i] * polynomial2[j];
	return result;
}

template <std::size_t M, std::size_t N>
inline auto
operator/(const basic_polynomial_t<M>& polynomial1, const basic_polynomial_t<N>& polynomial2)
{
	basic_polynomial_t<M> remain = polynomial1;
	basic_polynomial_t<M-N+1> quotient;
	for (std::size_t i = polynomial1.size() - polynomial2.size(); i != std::size_t(-1); --i)
	{
		quotient[i] = remain[i + polynomial2.size() - 1] / polynomial2.back();
		for (std::size_t j = polynomial2.size() - 1; j != std::size_t(-1); --j)
		{
			remain[i + j] -= quotient[i] * polynomial2[j];
		}
	}
	return quotient;
}

template <std::size_t N>
inline auto
evaluate(const basic_polynomial_t<N>& polynomial, const coeff_t x)
{
	return std::accumulate
	(
		polynomial.crbegin() + 1, polynomial.crend(),
		polynomial.back(),
		std::bind(std::fma<coeff_t,coeff_t,coeff_t>, x, std::placeholders::_1, std::placeholders::_2)
	);
}

template <std::size_t N>
inline auto
differentiate(const basic_polynomial_t<N>& polynomial)
{
	basic_polynomial_t<N-1> result;
	for (std::size_t index = N - 1; index > 0; --index)
		result[index - 1] = index * polynomial[index];
	return result;
}

template <typename Iterator>
bool
bairstow(const Iterator begin, const Iterator end, polynomial2_t& a)
{
	constexpr coeff_t e = 1e-3;
	constexpr std::size_t I = 100;

	for (std::size_t i = 0; i < I; ++i)
	{
		coeff_t b0 = *begin;
		coeff_t b1 = 0;
		coeff_t q0 = 0;
		coeff_t q1 = 0;

		std::for_each
		(
			begin + 1, end,
			[&](const coeff_t ci)
			{
				const coeff_t b2 = b1;
				b1 = b0;
				b0 = ci - a[1] * b1 - a[0] * b2;

				const coeff_t q2 = q1;
				q1 = q0;
				q0 = b2 - a[1] * q1 - a[0] * q2;
			}
		);

		if (std::abs(b0) < e && std::abs(b1) < e)
			return true;

		const coeff_t M  =  -a[0] * q1 - a[1]  * q0;
		const coeff_t D  =  q0 * q0 - M  * q1;
		const coeff_t da1 = (b0 * q1 - b1 * q0) / D;
		const coeff_t da0 = (b1 * M  - b0 * q0) / D;

		a[1] -= da1;
		a[0] -= da0;
	}

	return false;
}

template <typename Iterator>
inline auto
solve(const polynomial1_t& polynomial, Iterator iterator)
{
	if (polynomial[1] != 0)
		*iterator++ = -polynomial[0] / polynomial[1];
	return iterator;
}

template <typename Iterator>
inline auto
solve(const polynomial2_t& polynomial, Iterator iterator)
{
	return iterator + gsl_poly_solve_quadratic
	(
		polynomial[2], polynomial[1], polynomial[0],
		iterator[0], iterator[1]
	);
}

template <typename Iterator>
inline auto
solve(const polynomial3_t& polynomial, Iterator iterator)
{
	return iterator + gsl_poly_solve_cubic
	(
		polynomial[2] / polynomial[3], polynomial[1] / polynomial[3], polynomial[0] / polynomial[3],
		iterator[0], iterator[1], iterator[2]
	);
}

template <typename Iterator>
inline auto
solve(const polynomial4_t& polynomial, Iterator iterator)
{
	return iterator + gsl_poly_solve_quartic
	(
		polynomial[3] / polynomial[4], polynomial[2] / polynomial[4], polynomial[1] / polynomial[4], polynomial[0] / polynomial[4],
		iterator[0], iterator[1], iterator[2], iterator[3]
	);
}

template <typename Iterator, std::size_t N>
inline auto
solve(const basic_polynomial_t<N>& polynomial, Iterator iterator)
{
	polynomial2_t q {{3, -2, 1}};

	if (bairstow(polynomial.crbegin(), polynomial.crend(), q))
	{
		const basic_polynomial_t<N-2> r = polynomial / q;

		iterator = solve(q, iterator);
		iterator = solve(r, iterator);
	}
	else
	{
		constexpr std::size_t M = N - 1;
		std::array<std::complex<coeff_t>, M> roots;
		std::array<coeff_t, M * M> matrix;
		gsl_poly_complex_workspace_float workspace {M, matrix.data()};
		if (gsl_poly_complex_solve(polynomial.data(), N, &workspace, reinterpret_cast<coeff_t*>(roots.data())) == 0)
			for (const std::complex<coeff_t>& root : roots)
				if (std::imag(root) == 0.0f)
					*iterator++ = std::real(root);
	}

	return iterator;
}

}
