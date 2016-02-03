/*
 * parser.hpp
 *
 *  Created on: 17.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "../parsing/vector.hpp"
#include "../parsing/variable/get.hpp"
#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace x3 = boost::spirit::x3;

BOOST_FUSION_ADAPT_STRUCT
(
    rt::texture::description_t,
	pigment,
	ambient,
	diffuse,
	specular,
	shininess,
	reflection
)

namespace rt {
namespace texture {
namespace detail {

x3::rule<class top_rule, description_t> const top_rule = "TextureTop";
x3::rule<class description_rule, description_t> const description_rule = "Texture";

auto const top_rule_def =
		description_rule [parsing::action::copy]
		|
		parsing::variable::get::rule [parsing::variable::get::as<description_t>()]
;

auto const description_rule_def =
		x3::lit("Texture") > x3::lit('{')
		>
		(x3::lit("pigment") > x3::lit('=') > parsing::vector::rule)
		>
		-(x3::lit("ambient") > x3::lit('=') > x3::float_)
		>
		-(x3::lit("diffuse") > x3::lit('=') > x3::float_)
		>
		-(x3::lit("specular") > x3::lit('=') > x3::float_)
		>
		-(x3::lit("shininess") > x3::lit('=') > x3::float_)
		>
		-(x3::lit("reflection") > x3::lit('=') > x3::float_)
		>
		x3::lit('}')
		;

BOOST_SPIRIT_DEFINE(top_rule, description_rule);

}

const auto& rule = detail::top_rule;

}
}
