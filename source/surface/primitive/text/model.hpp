/*
 * model.hpp
 *
 *  Created on: 13.02.20016
 *      Author: mike_gresens
 */

#pragma once

#include <math/bezier.hpp>
#include <math/polynomial.hpp>
#include <geo/segment.hpp>
#include <geo/box.hpp>
#include <boost/variant.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/function_output_iterator.hpp>
#include <boost/range/numeric.hpp>
#include <vector>

namespace rt {
namespace surface {
namespace primitive {
namespace text {

template <std::size_t N>
using basic_contour_t = std::tuple<basic_bezier_t<N>, basic_bezier_t<N-1>>;

typedef basic_contour_t<2> contour1_t;
typedef basic_contour_t<3> contour2_t;
typedef basic_contour_t<4> contour3_t;

typedef boost::variant<contour1_t, contour2_t, contour3_t> contour_t;
typedef std::vector<contour_t> contours_t;

class solve_contour_visitor
:
	public boost::static_visitor<>
{
	typedef boost::array<float, 3> roots_t;

	class check_range
	{
	public:
		constexpr bool
		operator()(const float x) const
		{
			return x >= 0.0f && x <= 1.0f;
		}
	};

public:
	typedef boost::filter_iterator<check_range, roots_t::const_iterator> const_iterator;

	solve_contour_visitor(const float dx, const float dy, const float c)
	:
		boost::static_visitor<>(),
		_dx(dx),
		_dy(dy),
		_c(c),
		_roots(),
		_end()
	{
	}

	template <std::size_t N>
	void
	operator()(const basic_contour_t<N>& contour) const
	{
		const basic_polynomial_t<N> polynomial = transform(std::get<0>(contour));
		_end = solve(polynomial, _roots.begin());
	}

	const_iterator begin() const
	{
		return boost::make_filter_iterator<check_range>(_roots.cbegin(), _end);
	}

	const_iterator end() const
	{
		return boost::make_filter_iterator<check_range>(_end, _end);
	}

protected:
	template <std::size_t N>
	basic_polynomial_t<N>
	transform(const basic_bezier_t<N>& bezier) const
	{
		basic_polynomial_t<N> transformed;
		boost::transform
		(
			bezier,
			transformed.begin(),
			[this](const vector_t& C)
			{
				return _dy * C[0] - _dx * C[1];
			}
		);
		transformed[0] += _c;
		return transformed;
	}

private:
	float _dx, _dy, _c;
	mutable roots_t _roots;
	mutable roots_t::const_iterator _end;
};

template <std::size_t M>
class evaluate_contour_visitor
:
	public boost::static_visitor<vector_t>
{
public:
	evaluate_contour_visitor(const float x)
	:
		boost::static_visitor<vector_t>(),
		_x(x)
	{
	}

	template <std::size_t N>
	vector_t
	operator()(const basic_contour_t<N>& contour) const
	{
		return evaluate(std::get<M>(contour), _x);
	}

private:
	float _x;
};

class glyph_t
{

public:
	glyph_t(contours_t&& contours, const float extrusion)
	:
		_contours(std::forward<contours_t>(contours)),
		_extrusion(extrusion)
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		if (std::abs(ray.direction[X]) < std::numeric_limits<float>::epsilon() && std::abs(ray.direction[Y]) < std::numeric_limits<float>::epsilon())
			return hits;

		const solve_contour_visitor solver
		(
			ray.direction[X],
			ray.direction[Y],
			ray.origin[Y] * ray.direction[X] - ray.origin[X] * ray.direction[Y]
		);

		for (const contour_t& contour : _contours)
		{
			boost::apply_visitor(solver, contour);
			for (const float root : solver)
			{
				const evaluate_contour_visitor<0> function_evaluater(root);
				const vector_t point = boost::apply_visitor(function_evaluater, contour);
				const float t = std::abs(ray.direction[X]) > std::abs(ray.direction[Y]) ?
						(point[X] - ray.origin[X]) / ray.direction[X] :
						(point[Y] - ray.origin[Y]) / ray.direction[Y];
				if (ray.min < t && t < ray.max)
				{
					const float z = ray.origin[Z] + ray.direction[Z] * t;
					if (z >= 0.0f && z <= _extrusion)
					{
						const evaluate_contour_visitor<1> derivation_evaluater(root);
						const vector_t normal = boost::apply_visitor(derivation_evaluater, contour);
						hits->distance = t;
						hits->normal = vector_t {-normal[Y], normal[X], 0.f, 0.f};
						++hits;
					}
				}
			}
		}

		return hits;
	}

	bool
	inside(const vector_t& point) const
	{
		const solve_contour_visitor solver(1.0f, 1.0f, point[Y] - point[X]);

		return boost::accumulate
		(
			_contours,
			0,
			[&point, &solver](const std::size_t crossings, const contour_t& contour)
			{
				boost::apply_visitor(solver, contour);

				return boost::accumulate
				(
					solver,
					crossings,
					[&point, &contour](const std::size_t crossings, const double root)
					{
						const evaluate_contour_visitor<0> evaluater(root);
						const vector_t p = boost::apply_visitor(evaluater, contour);
						return crossings + (p[X] > point[X] && p[Y] > point[Y]);
					}
				);
			}
		) & 1;	// odd = inside, even = outside
	}

private:
	contours_t _contours;
	float _extrusion;
};

typedef std::vector<glyph_t> glyphs_t;
typedef std::pair<box_t, std::uint32_t> value_t;
typedef geo::index::rtree<value_t, geo::index::rstar<8>> rtree_t; // TODO: parse parameter

class model_t
{
public:
	model_t(glyphs_t&& glyphs, rtree_t&& rtree, const float extrusion)
	:
		_glyphs(std::forward<glyphs_t>(glyphs)),
		_rtree(std::forward<rtree_t>(rtree)),
		_extrusion(extrusion)
	{
	}

	rendering::hits_t::iterator
	hit(const rendering::ray_t& ray, rendering::hits_t::iterator hits) const
	{
		if (std::abs(ray.direction[Z]) > std::numeric_limits<float>::epsilon())
		{
			const float distance1 = -ray.origin[Z] / ray.direction[Z];
			if (ray.min < distance1 && distance1 < ray.max)
			{
				const vector_t point = ray.origin + ray.direction * distance1;
				const auto test = [this, &point, &hits, distance1](const value_t& value)
				{
					const glyph_t& glyph = _glyphs[value.second];
					if (glyph.inside(point))
					{
						hits->distance = distance1;
						hits->normal = vector_t {0, 0, -1, 0};
						++hits;
					}
				};

				_rtree.query(geo::index::intersects(to_point(point)), boost::make_function_output_iterator(test));
			}

			const float distance2 = (_extrusion - ray.origin[Z]) / ray.direction[Z];
			if (ray.min < distance2 && distance2 < ray.max)
			{
				const vector_t point = ray.origin + ray.direction * distance2;
				const auto test = [this, &point, &hits, distance2](const value_t& value)
				{
					const glyph_t& glyph = _glyphs[value.second];
					if (glyph.inside(point))
					{
						hits->distance = distance2;
						hits->normal = vector_t {0, 0, +1, 0};
						++hits;
					}
				};

				_rtree.query(geo::index::intersects(to_point(point)), boost::make_function_output_iterator(test));
			}
		}

		const auto test = [this, &ray, &hits](const value_t& value)
		{
			const glyph_t& glyph = _glyphs[value.second];
			hits = glyph.hit(ray, hits);
		};

		const segment_t segment = to_segment(ray);
		_rtree.query(geo::index::intersects(segment), boost::make_function_output_iterator(test));

		return hits;
	}

	bool
	inside(const vector_t& point) const
	{
		return std::any_of
		(
			_rtree.qbegin(geo::index::intersects(to_point(point))), _rtree.qend(),
			[this, &point](const value_t& value)
			{
				const glyph_t& glyph = _glyphs[value.second];
				return glyph.inside(point);
			}
		);
	}

private:
	glyphs_t _glyphs;
	rtree_t _rtree;
	float _extrusion;
};

}
}
}
}
