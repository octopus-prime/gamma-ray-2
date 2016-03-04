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
	rt::surface::primitive::mesh::description_t,
	file
)

namespace rt {
namespace surface {
namespace primitive {
namespace mesh {

x3::rule<class description_rule, description_t> const description_rule = "Mesh";

auto const description_rule_def =
		x3::lit("Mesh") > x3::lit('{')
		>
		x3::lit("file") > x3::lit('=') > parsing::filename::rule
		>
		x3::lit('}')
		;

BOOST_SPIRIT_DEFINE(description_rule);

}
}
}
}
