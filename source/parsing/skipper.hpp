/*
 * skipper.hpp
 *
 *  Created on: 18.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

namespace rt {
namespace parsing {
namespace skipper {
namespace detail {

x3::rule<class skip_rule> const skip_rule;
x3::rule<class line_rule> const line_rule;
x3::rule<class block_rule> const block_rule;

auto const skip_rule_def =
		x3::space
		|
		line_rule
		|
		block_rule
;

auto const line_rule_def =
		x3::lit("//") >> *(x3::char_ - x3::eol) >> x3::eol
;

auto const block_rule_def =
		x3::lit("/*") >> *(x3::char_ - x3::lit("*/")) >> x3::lit("*/")
;

BOOST_SPIRIT_DEFINE(skip_rule, line_rule, block_rule);

}

auto const& rule = detail::skip_rule;

}
}
}
