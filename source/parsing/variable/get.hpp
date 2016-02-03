/*
 * get.hpp
 *
 *  Created on: 17.11.2015
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
namespace get {
namespace detail {

//class tag {};

x3::rule<class variable_rule, /*const*/ description_t*> const variable_rule = "Variable";

auto const variable_rule_action = [](auto& ctx)
{
	descriptions_t& map = x3::get<tag>(ctx);
	const auto found = map.find(x3::_attr(ctx));
	if (found != map.end())
	{
		x3::_val(ctx) = &found->second;
		x3::_pass(ctx) = true;
	}
	else
	{
		x3::_val(ctx) = nullptr;
		x3::_pass(ctx) = false;
	}
};

auto const variable_rule_def =
		identifier::rule [variable_rule_action];

BOOST_SPIRIT_DEFINE(variable_rule);

}

template <typename T>
struct as
{
    void operator()(auto& ctx) const
    {
		try
		{
			x3::_val(ctx) = boost::get<T>(*x3::_attr(ctx));
			x3::_pass(ctx) = true;
		}
		catch (...)
		{
			x3::_pass(ctx) = false;
		}
	}
};

const auto& rule = detail::variable_rule;

}
}
}
}
