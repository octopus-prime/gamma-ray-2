/*
 * parser.hpp
 *
 *  Created on: 28.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "primitive/parser.hpp"
#include "../parsing/variable/get.hpp"
#include "../parsing/action/copy.hpp"
#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

namespace rt {
namespace surface {
namespace detail {

template <typename Tag>
struct make
{
    void operator()(auto& ctx) const
    {
    	const csg::description_t<Tag> description = {x3::_val(ctx), x3::_attr(ctx)};
    	x3::_val(ctx) = description;
	}
};

x3::rule<class description_rule, description_t> const description_rule = "Surface";
x3::rule<class term_rule, description_t> const term_rule = "Term";

auto const description_rule_def =
		term_rule 											[parsing::action::copy]
		>
		*(
				(x3::lit('+') > term_rule)					[make<csg::union_tag>()]
				|
				(x3::lit('-') > term_rule)					[make<csg::difference_tag>()]
				|
				(x3::lit('&') > term_rule)					[make<csg::intersection_tag>()]
		)
		;

auto const term_rule_def =
		primitive::description_rule							[parsing::action::copy]
		|
		parsing::variable::get::rule						[parsing::variable::get::as<description_t>()]
		|
		(x3::lit('(') > description_rule > x3::lit(')'))	[parsing::action::copy]
		;

BOOST_SPIRIT_DEFINE(description_rule, term_rule);

}

const auto& rule = detail::description_rule;

}
}
