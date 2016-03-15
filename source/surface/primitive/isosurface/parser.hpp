/*
 * parser.hpp
 *
 *  Created on: 13.02.2016
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "../../../parsing/vector.hpp"
#include "../../../parsing/action/copy.hpp"
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace x3 = boost::spirit::x3;

BOOST_FUSION_ADAPT_STRUCT
(
	rt::surface::primitive::isosurface::description_t,
	function,
	min,
	max
)

namespace rt {
namespace surface {
namespace primitive {
namespace isosurface {

x3::rule<class description_rule, description_t> const description_rule = "Isosurface";
x3::rule<class expression_rule, function_t> const expression_rule = "Expression";
x3::rule<class term_rule, function_t> const term_rule = "Term";
x3::rule<class factor_rule, function_t> const factor_rule = "Factor";
x3::rule<class unary_rule, function_t> const unary_rule = "Unary";
x3::rule<class binary_rule, function_t> const binary_rule = "Binary";
x3::rule<class value_rule, function_t> const value_rule = "Value";
x3::rule<class variable_rule, function_t> const variable_rule = "Variable";

x3::symbols<size_t> const variable_sym
{
	{
		{"x", X},
		{"y", Y},
		{"z", Z},
	},
	"Variable"
};

x3::symbols<std::function<float (float)>> const unary_sym
{
	{
		{"sqrt", sqrtf},
		{"cbrt", cbrtf},
		{"sin", sinf},
		{"cos", cosf},
		{"tan", tanf},
		{"abs", fabsf},
		{"log", logf},
		{"exp", expf},
	},
	"Unary"
};

x3::symbols<std::function<float (float, float)>> const binary_sym
{
	{
		{"min", fminf},
		{"max", fmaxf},
		{"pow", powf},
	},
	"Binary"
};

const auto negate_action = [](auto& ctx)
{
	const auto& function = x3::_attr(ctx);
	x3::_val(ctx) = [function](const vector_t& point)
	{
		return -function(point);
	};
};

const auto unary_action = [](auto& ctx)
{
	using boost::fusion::at_c;
	const auto& function = at_c<0>(x3::_attr(ctx));
	const auto& function1 = at_c<1>(x3::_attr(ctx));
	x3::_val(ctx) = [function, function1](const vector_t& point)
	{
		return function(function1(point));
	};
};

const auto binary_action = [](auto& ctx)
{
	using boost::fusion::at_c;
	const auto& function = at_c<0>(x3::_attr(ctx));
	const auto& function1 = at_c<1>(x3::_attr(ctx));
	const auto& function2 = at_c<2>(x3::_attr(ctx));
	x3::_val(ctx) = [function, function1, function2](const vector_t& point)
	{
		return function(function1(point), function2(point));
	};
};

const auto variable_action = [](auto& ctx)
{
	const size_t index = x3::_attr(ctx);
	x3::_val(ctx) = [index](const vector_t& point)
	{
		return point[index];
	};
};

const auto value_action = [](auto& ctx)
{
	const float value = x3::_attr(ctx);
	x3::_val(ctx) = [value](const vector_t& point)
	{
		return value;
	};
};

template <template <typename> class Op>
struct calc_action
{
	template <typename Ctx>
	void operator()(Ctx& ctx) const
	{
		const function_t& function1 = x3::_val(ctx);
		const function_t& function2 = x3::_attr(ctx);
		x3::_val(ctx) = [function1, function2](const vector_t& point)
		{
			return Op<float>()(function1(point), function2(point));
		};
	}
};

auto const description_rule_def =
		x3::lit("Isosurface") > x3::lit('{')
		>
		x3::lit("function") > x3::lit('=') > expression_rule
		>
		-(x3::lit("min") > x3::lit('=') > parsing::vector::rule)
		>
		-(x3::lit("max") > x3::lit('=') > parsing::vector::rule)
		>
		x3::lit('}')
		;

auto const expression_rule_def =
		term_rule 											[parsing::action::copy]
		>
		*(
			(x3::lit('+') > term_rule)						[calc_action<std::plus>()]
			|
			(x3::lit('-') > term_rule)						[calc_action<std::minus>()]
		)
;

auto const term_rule_def =
		factor_rule											[parsing::action::copy]
		>
		*(
			(x3::lit('*') > factor_rule)					[calc_action<std::multiplies>()]
			|
			(x3::lit('/') > factor_rule)					[calc_action<std::divides>()]
		)
;

auto const factor_rule_def =
		value_rule											[parsing::action::copy]
		|
		variable_rule										[parsing::action::copy]
		|
		unary_rule											[parsing::action::copy]
		|
		binary_rule											[parsing::action::copy]
		|
		(x3::lit('(') > expression_rule > x3::lit(')'))		[parsing::action::copy]
		|
		(x3::lit('+') > factor_rule)						[parsing::action::copy]
		|
		(x3::lit('-') > factor_rule)						[negate_action]
		;

auto const variable_rule_def =
		variable_sym [variable_action]
	;

auto const unary_rule_def =
		(unary_sym > x3::lit('(') > expression_rule > x3::lit(')')) [unary_action]
	;

auto const binary_rule_def =
		(binary_sym > x3::lit('(') > expression_rule > x3::lit(',') > expression_rule > x3::lit(')')) [binary_action]
	;

auto const value_rule_def =
		x3::float_ [value_action]
		;

BOOST_SPIRIT_DEFINE(description_rule, expression_rule, term_rule, factor_rule, binary_rule, unary_rule, value_rule, variable_rule);

}
}
}
}
