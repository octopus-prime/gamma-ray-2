/*
 * parser.hpp
 *
 *  Created on: 26.12.2016
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "../../../parsing/vector.hpp"
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_pair.hpp>

namespace x3 = boost::spirit::x3;

BOOST_FUSION_ADAPT_STRUCT
(
	rt::surface::primitive::blob::description_t,
	carriers,
	threshold
)

namespace rt {
namespace surface {
namespace primitive {
namespace blob {

x3::rule<class description_rule, description_t> const description_rule = "Blob";
x3::rule<class carriers_rule, carriers_t> const carriers_rule = "Carriers";
x3::rule<class carrier_rule, carrier_t> const carrier_rule = "Carrier";

auto const description_rule_def =
		x3::lit("Blob") > x3::lit('{')
		>
		x3::lit("carriers") > x3::lit('=') > carriers_rule
		>
		x3::lit("threshold") > x3::lit('=') > x3::float_
		>
		x3::lit('}')
		;

auto const carriers_rule_def = x3::lit('[') > carrier_rule % x3::lit(',') > x3::lit(']');

auto const carrier_rule_def = x3::lit('{') > parsing::vector::rule > x3::lit(',') > x3::float_ > x3::lit('}');

BOOST_SPIRIT_DEFINE(description_rule, carriers_rule, carrier_rule);

}
}
}
}
