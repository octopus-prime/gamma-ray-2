/*
 * set.hpp
 *
 *  Created on: 18.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "description.hpp"
#include "../identifier.hpp"
#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

namespace rt {
namespace parsing {
namespace variable {
namespace set {
namespace detail {

x3::rule<class set_rule> const set_rule = "Set";
x3::rule<class variable_rule, description_t> const variable_rule = "Variable";

auto const set_action = [](auto& ctx)
{
	const auto& key = boost::fusion::at_c<0>(x3::_attr(ctx));
	const auto& val = boost::fusion::at_c<1>(x3::_attr(ctx));
	descriptions_t& variables = x3::get<tag>(ctx);
	variables.emplace(key, val);
};

auto const set_rule_def =
	(
	x3::lit("var")
	>
	identifier::rule
	>
	x3::lit('=')
	>
	variable_rule
	) [set_action]
;

auto const variable_rule_def =
	vector::rule
	|
	texture::rule
	|
	surface::rule
	|
	scene::camera::rule
	|
	scene::light::rule
	|
	scene::object::rule
	|
	scene::rule
;

BOOST_SPIRIT_DEFINE(variable_rule, set_rule);

}

const auto& rule = detail::set_rule;

}
}
}
}
