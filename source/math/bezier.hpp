/*
 * bezier.hpp
 *
 *  Created on: 06.03.2015
 *      Author: mike_gresens
 */

#pragma once

#include <math/vector.hpp>
#include <boost/math/special_functions/factorials.hpp>

namespace rt {

template <std::size_t N>
using basic_bezier_t = std::array<vector_t, N>;

typedef basic_bezier_t<2> bezier1_t;
typedef basic_bezier_t<3> bezier2_t;
typedef basic_bezier_t<4> bezier3_t;

template <std::size_t N>
vector_t
evaluate(const basic_bezier_t<N>& bezier, const float x)
{
	return std::accumulate
	(
		bezier.crbegin() + 1, bezier.crend(),
		bezier.back(),
		[x](const vector_t& y, const vector_t& c)
		{
			return x * y + c;
		}
	);
}

template <std::size_t N>
basic_bezier_t<N-1>
differentiate(const basic_bezier_t<N>& bezier)
{
	basic_bezier_t<N-1> result;
	std::transform
	(
		bezier.cbegin(), bezier.cend() - 1,
		bezier.cbegin() + 1,
		result.begin(),
		[](const vector_t& p0, const vector_t& p1)
		{
			return N * (p1 - p0);
		}
	);
	return result;
}

template <std::size_t N>
basic_bezier_t<N>
to_polynomial(const basic_bezier_t<N>& bezier)
{
	constexpr std::size_t n = N - 1;
	basic_bezier_t<N> p;
	const float nF = boost::math::factorial<float>(n);
	for (std::size_t j = 0; j <= n; ++j)
	{
		const float njF = boost::math::factorial<float>(n - j);
		const float factor = nF / njF;
		vector_t sum(0);
		for (std::size_t i = 0; i <= j; ++i)
		{
			const float s = std::pow(-1.0f, i + j);
			const float iF = boost::math::factorial<float>(i);
			const float jiF = boost::math::factorial<float>(j - i);
			sum += (s * bezier[i]) / (iF * jiF);
		}
		p[j] = factor * sum;
	}
	return p;
}

}
