/*
 * parser.hpp
 *
 *  Created on: 13.02.2016
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
	rt::surface::primitive::box::description_t,
	min,
	max
)

namespace rt {
namespace surface {
namespace primitive {
namespace box {

x3::rule<class description_rule, description_t> const description_rule = "Box";

auto const description_rule_def =
		x3::lit("Box") > x3::lit('{')
		>
		-(x3::lit("min") > x3::lit('=') > parsing::vector::rule)
		>
		-(x3::lit("max") > x3::lit('=') > parsing::vector::rule)
		>
		x3::lit('}')
		;

BOOST_SPIRIT_DEFINE(description_rule);

}
}
}
}
