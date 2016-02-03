/*
 * configuration.cpp
 *
 *  Created on: 25.02.2015
 *      Author: mike_gresens
 */

#include "configuration.hpp"
#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <fstream>
#include <iostream>

namespace po = boost::program_options;

namespace rt {

static const std::string CONFIG("config");
static const std::string INPUT("input");
static const std::string OUTPUT("output");
static const std::string WIDTH("width");
static const std::string HEIGHT("height");
static const std::string DEPTH("depth");
static const std::string AA("aa");
static const std::string LEVEL("level");

configuration_t
configure(const int argc, const char* const argv[])
{
	po::options_description description;

	po::options_description req_options("Required options");
	req_options.add_options()
		((INPUT  + ",i").c_str(),	po::value<std::string>()->required(),			"input file")
		((OUTPUT + ",o").c_str(),	po::value<std::string>()->required(),			"output file")
	;

	po::options_description opt_options("Optional options");
	opt_options.add_options()
		((CONFIG + ",c").c_str(),	po::value<std::string>()->default_value(""),	"config file")
		((WIDTH  + ",w").c_str(),	po::value<std::size_t>()->default_value(1920),	"resolution width")
		((HEIGHT + ",h").c_str(),	po::value<std::size_t>()->default_value(1080),	"resolution height")
		((DEPTH  + ",d").c_str(),	po::value<std::size_t>()->default_value(8),		"rendering depth")
		((AA     + ",a").c_str(),	po::bool_switch()->default_value(false), 		"anti aliasing")
		((LEVEL  + ",l").c_str(),	po::value<std::string>()->default_value("i"),	"log level")
	;

	description.add(req_options);
	description.add(opt_options);

	if (argc == 1)
	{
		std::cout << std::endl;
		std::cout << "Usage: " << argv[0] << " <options>" << std::endl;
		std::cout << description << std::endl;
		exit(EXIT_SUCCESS);
	}

	po::variables_map variables;

	po::store(po::parse_command_line(argc, argv, description), variables);
	po::notify(variables);

	const auto config = variables.at(CONFIG).as<std::string>();
	if (!config.empty())
	{
		std::ifstream file(config);
		if (file.fail())
			throw std::runtime_error(config + " not found.");
		po::store(po::parse_config_file(file, description), variables);
		file.close();
		po::notify(variables);
	}

	return configuration_t
	{
		variables.at(INPUT).as<std::string>(),
		variables.at(OUTPUT).as<std::string>(),
		variables.at(WIDTH).as<std::size_t>(),
		variables.at(HEIGHT).as<std::size_t>(),
		variables.at(DEPTH).as<std::size_t>(),
		variables.at(AA).as<bool>(),
		variables.at(LEVEL).as<std::string>()
	};
}

}
