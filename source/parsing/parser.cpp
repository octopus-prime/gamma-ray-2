/*
 * parser.cpp
 *
 *  Created on: 12.12.2015
 *      Author: mike_gresens
 */

#include "parser.hpp"
#include "../scene/parser.hpp"
#include "variable/get.hpp"
#include "variable/set.hpp"
#include "skipper.hpp"
#include "identifier.hpp"
#include "filename.hpp"
#include "transformation.hpp"
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <fstream>

namespace rt {
namespace parsing {

template <typename Type, typename Rule>
class parser_t
{
public:
	parser_t(const Rule& rule,	variable::descriptions_t& variables)
	:
		_rule(rule),
		_variables(variables)
	{
	}

	Type operator()(const std::string& file) const
	{
//		BOOST_LOG_TRIVIAL(debug) << "Parse file " << '"' << file << '"';

		std::ifstream stream(file);
		stream.unsetf(std::ios::skipws);

		std::string text;
		std::copy(std::istream_iterator<char>(stream), std::istream_iterator<char>(), std::back_inserter(text));

		scene::description_t description;
		auto rule = x3::with<variable::tag>(std::ref(_variables)) [x3::eps > _rule];

//		try
//		{
			auto iter = text.begin();

			const bool ok = x3::phrase_parse
			(
				iter, text.end(),
				rule,
				skipper::rule,
				description
			);

			if (!ok || iter != text.end())
				throw std::runtime_error("Parsing failed.");
//		}
//		catch (const x3::expectation_failure<std::string::iterator>& failure)
//		{
//			boost::format what("Expected %s but got %s");
//			what % failure.which();
//			what % std::string(failure.where(), text.end());
//			throw std::runtime_error(what.str());
//		}

		return description;
	}

private:
	const Rule& _rule;
	variable::descriptions_t& _variables;
};

template <typename Type, typename Rule>
parser_t<Type, Rule>
make_parser(const Rule& rule, variable::descriptions_t& variables)
{
	return parser_t<Type, Rule>(rule, variables);
}

x3::rule<class def_rule> const def_rule ="Definition";
x3::rule<class include_rule> const include_rule = "Include";

auto const def_rule_def =
		variable::set::rule
		|
		include_rule
		|
		transformation::rule
;

auto const include_action = [](auto& ctx)
{
	variable::descriptions_t& variables = x3::get<variable::tag>(ctx);
	const auto parser = make_parser<x3::unused_type>(*def_rule, variables);
	const auto& file = x3::_attr(ctx);
//	try
//	{
//	BOOST_LOG_TRIVIAL(debug) << "Parse file " << '"' << file << '"';
//	}
//	catch (...)
//	{
//		std::cerr << "Foo1" << std::endl;
//	}
	return parser(file);
};

auto const include_rule_def =
		(x3::lit("include") > filename::rule) [include_action]
;

BOOST_SPIRIT_DEFINE(def_rule, include_rule);

scene::description_t
parse(const std::string& file)
{
	variable::descriptions_t variables;
	const auto parser = make_parser<scene::description_t>(*def_rule > x3::lit("render") > scene::rule, variables);
//	try
//	{
//	BOOST_LOG_TRIVIAL(debug) << "Parse file " << '"' << file << '"';
//	}
//	catch (...)
//	{
//		std::cerr << "Foo2" << std::endl;
//	}
	return parser(file);
}

}
}
