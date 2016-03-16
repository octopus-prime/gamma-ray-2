/*
 * vector.hpp
 *
 *  Created on: 18.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "action/copy.hpp"
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include "variable/get.hpp"

namespace x3 = boost::spirit::x3;

BOOST_FUSION_DEFINE_STRUCT
(
	(rt)(parsing)(vector)(detail), description_t,
	(float, x)
	(float, y)
	(float, z)
)

namespace rt {
namespace parsing {
namespace vector {
namespace detail {

x3::rule<class elements_rule, description_t> const elements_rule = "Elements";
x3::rule<class factor_rule, vector_t> const factor_rule = "Factor";
x3::rule<class term_rule, vector_t> const term_rule = "Term";
x3::rule<class expression_rule, vector_t> const expression_rule = "Expression";

auto const set = [](auto& ctx)
{
	const description_t& d = x3::_attr(ctx);
	const vector_t vector {d.x, d.y, d.z, 0};
	x3::_val(ctx) = vector;
};
auto const set2 = [](auto& ctx)
{
	const float v = x3::_attr(ctx);
	const vector_t vector {v, v, v, 0};
	x3::_val(ctx) = vector;
};
auto const neg = [](auto& ctx)
{
	x3::_val(ctx) = -x3::_attr(ctx);
};
template <template <typename> class Op>
struct calc_action
{
	void operator()(auto& ctx) const
	{
		x3::_val(ctx) = Op<vector_t>()(x3::_val(ctx), x3::_attr(ctx));
	}
};

auto const elements_rule_def =
		x3::lit('<')
		>
//		x3::repeat(3) [x3::float_]
		x3::float_ > x3::lit(',')
		>
		x3::float_ > x3::lit(',')
		>
		x3::float_
		>
		x3::lit('>')
		;

auto const factor_rule_def =
		elements_rule									[set]
		|
		x3::float_										[set2]
		|
		variable::get::rule								[variable::get::as<vector_t>()]
		|
		(x3::lit('(') > expression_rule > x3::lit(')')) [action::copy]
		|
		(x3::lit('+') > factor_rule) 					[action::copy]
		|
		(x3::lit('-') > factor_rule)					[neg]
;

auto const term_rule_def =
		factor_rule										[action::copy]
		>
		*(
				(x3::lit('*') > factor_rule)			[calc_action<std::multiplies>()]
				|
				(x3::lit('/') > factor_rule)			[calc_action<std::divides>()]
		)
;

auto const expression_rule_def =
		term_rule 										[action::copy]
		>
		*(
				(x3::lit('+') > term_rule)				[calc_action<std::plus>()]
				|
				(x3::lit('-') > term_rule)				[calc_action<std::minus>()]
		)
;

BOOST_SPIRIT_DEFINE(expression_rule, term_rule, factor_rule, elements_rule);

}

const auto& rule = detail::expression_rule;

}
}
}
