/*
 * parser.hpp
 *
 *  Created on: 28.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "sphere/parser.hpp"
#include "plane/parser.hpp"
#include "quadric/parser.hpp"
#include "torus/parser.hpp"
#include "sor/parser.hpp"
#include "box/parser.hpp"
#include "text/parser.hpp"
#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

namespace rt {
namespace surface {
namespace primitive {

x3::rule<class description_rule, description_t> const description_rule = "Surface";

auto const description_rule_def =
		sphere::description_rule //[parsing::vector::cpy]
		|
		plane::description_rule //[parsing::vector::cpy]
		|
		quadric::description_rule
		|
		torus::description_rule
		|
		sor::description_rule
		|
		box::description_rule
		|
		text::description_rule
		;

BOOST_SPIRIT_DEFINE(description_rule);

}
}
}
