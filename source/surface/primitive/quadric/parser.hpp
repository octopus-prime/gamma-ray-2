/*
 * parser.hpp
 *
 *  Created on: 17.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "../../../parsing/vector.hpp"
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace x3 = boost::spirit::x3;

BOOST_FUSION_ADAPT_STRUCT
(
	rt::surface::primitive::quadric::description_t,
	square,
	linear,
	absolute,
	constant
)

namespace rt {
namespace surface {
namespace primitive {
namespace quadric {

x3::rule<class description_rule, description_t> const description_rule = "Quadric";

auto const description_rule_def =
		x3::lit("Quadric") > x3::lit('{')
		>
		x3::lit("square") > x3::lit('=') > parsing::vector::rule
		>
		x3::lit("linear") > x3::lit('=') > parsing::vector::rule
		>
		x3::lit("absolute") > x3::lit('=') > parsing::vector::rule
		>
		x3::lit("constant") > x3::lit('=') > x3::float_
		>
		x3::lit('}')
		;

BOOST_SPIRIT_DEFINE(description_rule);

}
}
}
}
