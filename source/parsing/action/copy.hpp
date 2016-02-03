/*
 * copy.hpp
 *
 *  Created on: 13.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

namespace rt {
namespace parsing {
namespace action {

auto const copy = [](auto& ctx)
{
	x3::_val(ctx) = x3::_attr(ctx);
};

}
}
}
