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
	rt::surface::primitive::torus::description_t,
	major,
	minor
)

namespace rt {
namespace surface {
namespace primitive {
namespace torus {

x3::rule<class description_rule, description_t> const description_rule = "Torus";

auto const description_rule_def =
		x3::lit("Torus") > x3::lit('{')
		>
		x3::lit("major") > x3::lit('=') > x3::float_
		>
		x3::lit("minor") > x3::lit('=') > x3::float_
		>
		x3::lit('}')
		;

BOOST_SPIRIT_DEFINE(description_rule);

}
}
}
}
