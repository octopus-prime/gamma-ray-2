/*
 * filename.hpp
 *
 *  Created on: 18.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

namespace rt {
namespace parsing {
namespace filename {
namespace detail {

x3::rule<class rule, std::string> const rule = "Filename";

auto const rule_def =
		x3::lit('"') >
		x3::lexeme[*(x3::char_ - x3::lit('"'))] >
		x3::lit('"')
;

BOOST_SPIRIT_DEFINE(rule);

}

auto const& rule = detail::rule;

}
}
}
