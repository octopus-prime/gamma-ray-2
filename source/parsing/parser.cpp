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
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <fstream>

namespace rt {
namespace parsing {

typedef boost::spirit::istream_iterator iterator_t;

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
		std::ifstream stream(file);
		stream.unsetf(std::ios::skipws);

		scene::description_t description;

		auto rule = x3::with<variable::tag>(std::ref(_variables)) [x3::eps > _rule];
		iterator_t iterator(stream);

		const bool ok = x3::phrase_parse
		(
			iterator, iterator_t(),
			rule,
			skipper::rule,
			description
		);

		if (!ok || iterator != iterator_t())
			throw std::runtime_error("Parsing failed.");

		return std::move(description);
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
x3::rule<class render_rule, scene::description_t> const render_rule = "Render";

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
	const auto& file = x3::_attr(ctx);
	try
	{
		BOOST_LOG_TRIVIAL(debug) << "Parse file " << '"' << file << '"';
		const auto parse = make_parser<x3::unused_type>(*def_rule, variables);
		return parse(file);
	}
	catch (const x3::expectation_failure<iterator_t>& failure)
	{
		boost::format what("Expected %s but got %s");
		what % failure.which();
		what % *failure.where();
		throw std::runtime_error(what.str());
	}
};

auto const include_rule_def =
		(x3::lit("include") > filename::rule) [include_action]
;

auto const render_rule_def =
		x3::lit("render") > x3::lit('(') > scene::rule > x3::lit(')')
;

BOOST_SPIRIT_DEFINE(def_rule, include_rule, render_rule);

scene::description_t
parse(const std::string& file)
{
	variable::descriptions_t variables;
	try
	{
		BOOST_LOG_TRIVIAL(debug) << "Parse file " << '"' << file << '"';
		const auto parse = make_parser<scene::description_t>(*def_rule > render_rule, variables);
		return parse(file);
	}
	catch (const x3::expectation_failure<iterator_t>& failure)
	{
		boost::format what("Expected %s but got %s");
		what % failure.which();
		what % *failure.where();
		throw std::runtime_error(what.str());
	}
}

}
}
