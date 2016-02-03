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
	rt::surface::primitive::sor::description_t,
	points
)

namespace rt {
namespace surface {
namespace primitive {
namespace sor {

x3::rule<class description_rule, description_t> const description_rule = "SoR";
x3::rule<class points_rule, std::vector<vector_t>> const points_rule = "Points";

auto const description_rule_def =
		x3::lit("SoR") > x3::lit('{')
		>
		x3::lit("points") > x3::lit('=') > points_rule
		>
		x3::lit('}')
		;

auto const points_rule_def =
		x3::lit('[')
		>
		*parsing::vector::rule
		>
		x3::lit(']')
		;

BOOST_SPIRIT_DEFINE(description_rule, points_rule);

}
}
}
}
