/*
 * configuration.hpp
 *
 *  Created on: 25.02.2015
 *      Author: mike_gresens
 */

#pragma once

#include <string>

namespace rt {

struct configuration_t
{
	const std::string input;
	const std::string output;
	const std::size_t width;
	const std::size_t height;
	const std::size_t depth;
	const std::size_t aa;
	const std::string level;
};

configuration_t
configure(const int argc, const char* const argv[]);

}
