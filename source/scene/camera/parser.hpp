/*
 * parser.hpp
 *
 *  Created on: 17.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "../../parsing/vector.hpp"
#include "../../parsing/variable/get.hpp"
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace x3 = boost::spirit::x3;

BOOST_FUSION_ADAPT_STRUCT
(
    rt::scene::camera::description_t,
	origin,
	direction,
	up
)

namespace rt {
namespace scene {
namespace camera {
namespace detail {

x3::rule<class top_rule, description_t> const top_rule = "CameraTop";
x3::rule<class description_rule, description_t> const description_rule = "Camera";

auto const top_rule_def =
		description_rule [parsing::action::copy]
		|
		parsing::variable::get::rule [parsing::variable::get::as<description_t>()]
;

auto const description_rule_def =
		x3::lit("Camera") > x3::lit('{')
		>
		x3::lit("origin") > x3::lit('=') > parsing::vector::rule
		>
		x3::lit("direction") > x3::lit('=') > parsing::vector::rule
		>
		-(x3::lit("up") > x3::lit('=') > parsing::vector::rule)
		>
		x3::lit('}')
		;

BOOST_SPIRIT_DEFINE(top_rule, description_rule);

}

const auto& rule = detail::top_rule;

}
}
}
