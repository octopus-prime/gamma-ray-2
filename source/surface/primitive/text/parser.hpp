/*
 * parser.hpp
 *
 *  Created on: 13.02.2016
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "../../../parsing/filename.hpp"
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace x3 = boost::spirit::x3;

BOOST_FUSION_ADAPT_STRUCT
(
	rt::surface::primitive::text::description_t,
	text,
	font,
	extrusion
)

namespace rt {
namespace surface {
namespace primitive {
namespace text {

x3::rule<class description_rule, description_t> const description_rule = "Text";

auto const description_rule_def =
		x3::lit("XTextY") > x3::lit('{')
		>
		x3::lit("glyphs") > x3::lit('=') > parsing::filename::rule
		>
		x3::lit("font") > x3::lit('=') > parsing::filename::rule
		>
		x3::lit("extrusion") > x3::lit('=') > x3::float_
		>
		x3::lit('}')
		;

BOOST_SPIRIT_DEFINE(description_rule);

}
}
}
}
