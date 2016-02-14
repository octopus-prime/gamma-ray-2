/*
 * transformation.hpp
 *
 *  Created on: 14.02.2016
 *      Author: mike_gresens
 */

#pragma once

#include <boost/spirit/home/x3.hpp>
#include "variable/get.hpp"
#include "../math/transformation.hpp"
#include <boost/variant/polymorphic_get.hpp>

namespace x3 = boost::spirit::x3;

namespace rt {
namespace parsing {
namespace transformation {
namespace detail {

x3::rule<class transform_rule> const transform_rule = "Transform";

class transformation_visitor
:
	public boost::static_visitor<>
{
public:
	transformation_visitor(const matrix_t& transformation)
	:
		_transformation(transformation)
	{
	}

	template <typename T>
	result_type operator()(T& transformable) const
	{
		throw std::runtime_error("Not transformable ");
	}

	result_type operator()(scene::object::description_t& transformable) const
	{
		(*this)(transformable.surface);
	}

	result_type operator()(surface::description_t& transformable) const
	{
		boost::apply_visitor(*this, transformable);
	}

	result_type operator()(surface::primitive::description_t& transformable) const
	{
		boost::polymorphic_get<surface::primitive::base_description_t>(transformable).transformation = _transformation * boost::polymorphic_get<surface::primitive::base_description_t>(transformable).transformation;
	}

	template <typename Tag>
	result_type operator()(surface::csg::description_t<Tag>& transformable) const
	{
		(*this)(transformable.surface1);
		(*this)(transformable.surface2);
	}

private:
	const matrix_t& _transformation;
};

auto const transform = [](auto& ctx)
{
	using boost::fusion::at_c;
	auto& transformable = *at_c<0>(x3::_attr(ctx));
	const auto transformation = at_c<1>(x3::_attr(ctx));
	vector_t vector = at_c<2>(x3::_attr(ctx));
	const matrix_t matrix = transformation(vector);
	const transformation_visitor visitor(matrix);
	boost::apply_visitor(visitor, transformable);
};

const x3::symbols<decltype(translation)*> transformation
{
	{"translate", &translation},
	{"rotate", &rotation},
	{"scale", &scaling}
};

auto const transform_rule_def =
		(variable::get::rule > x3::lit('.') > transformation > x3::lit('(') > vector::rule > x3::lit(')')) [transform]
;

BOOST_SPIRIT_DEFINE(transform_rule);

}

const auto& rule = detail::transform_rule;

}
}
}
