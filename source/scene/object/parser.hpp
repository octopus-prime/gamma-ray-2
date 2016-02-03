/*
 * parser.hpp
 *
 *  Created on: 17.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "../../parsing/vector.hpp"
#include "../../surface/parser.hpp"
#include "../../texture/parser.hpp"
#include "../../parsing/variable/get.hpp"
#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

BOOST_FUSION_ADAPT_STRUCT
(
    rt::scene::object::description_t,
	surface,
	texture
)

namespace rt {
namespace scene {
namespace object {
namespace detail {

x3::rule<class top_rule, description_t> const top_rule = "ObjectTop";
x3::rule<class description_rule, description_t> const description_rule = "Object";

auto const top_rule_def =
		description_rule [parsing::action::copy]
		|
		parsing::variable::get::rule [parsing::variable::get::as<description_t>()]
;

auto const description_rule_def =
		x3::lit("Object") > x3::lit('{')
		>
		x3::lit("surface") > x3::lit('=') > surface::rule
		>
		x3::lit("texture") > x3::lit('=') > texture::rule
		>
		x3::lit('}')
		;

BOOST_SPIRIT_DEFINE(top_rule, description_rule);

}

const auto& rule = detail::top_rule;

}
}
}
