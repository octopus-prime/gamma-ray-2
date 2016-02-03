/*
 * parser.hpp
 *
 *  Created on: 17.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "camera/parser.hpp"
#include "light/parser.hpp"
#include "object/parser.hpp"
#include "../parsing/variable/get.hpp"
#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

BOOST_FUSION_ADAPT_STRUCT
(
    rt::scene::description_t,
    camera,
    lights,
    objects
)

namespace rt {
namespace scene {
namespace detail {

x3::rule<class top_rule, description_t> const top_rule = "SceneTop";
x3::rule<class description_rule, description_t> const description_rule = "Scene";
x3::rule<class lights_rule, light::descriptions_t> const lights_rule = "Lights";
x3::rule<class objects_rule, object::descriptions_t> const objects_rule = "Objects";

auto const top_rule_def =
		description_rule [parsing::action::copy]
		|
		parsing::variable::get::rule [parsing::variable::get::as<description_t>()]
;

auto const description_rule_def =
		x3::lit("Scene") > x3::lit('{')
		>
		x3::lit("camera") > x3::lit('=') > camera::rule
		>
		x3::lit("lights") > x3::lit('=') > lights_rule
		>
		x3::lit("objects") > x3::lit('=') > objects_rule
		>
		x3::lit('}')
		;

auto const lights_rule_def =
		x3::lit("[")
		>
//		*(light::rule - x3::lit("]"))
		*light::rule
		>
		x3::lit(']')
		;

auto const objects_rule_def =
		x3::lit("[")
		>
//		*(object::rule - x3::lit("]"))
		*object::rule
		>
		x3::lit(']')
		;

BOOST_SPIRIT_DEFINE(top_rule, description_rule, lights_rule, objects_rule);

}

const auto& rule = detail::top_rule;

}
}
