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
#include "../math/transformation.hpp"
#include <boost/variant/polymorphic_get.hpp>
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
x3::rule<class transform_rule> const transform_rule = "Transform";

auto const def_rule_def =
		variable::set::rule
		|
		include_rule
		|
		transform_rule
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

class transformation_visitor
:
	public boost::static_visitor<>
{
public:
	transformation_visitor(const matrix_t& transformation)
	:
		_transformation(transformation)
	{
	}

	template <typename T>
	result_type operator()(T& transformable) const
	{
		throw std::runtime_error("Not transformable ");
	}

	result_type operator()(scene::object::description_t& transformable) const
	{
		(*this)(transformable.surface);
	}

	result_type operator()(surface::description_t& transformable) const
	{
		boost::apply_visitor(*this, transformable);
	}

	result_type operator()(surface::primitive::description_t& transformable) const
	{
#ifdef COL_MAJOR
		boost::polymorphic_get<surface::primitive::base_description_t>(transformable).transformation = _transformation * boost::polymorphic_get<surface::primitive::base_description_t>(transformable).transformation;
#else
		boost::polymorphic_get<surface::primitive::base_description_t>(transformable).transformation *= _transformation;
#endif
	}

	template <typename Tag>
	result_type operator()(surface::csg::description_t<Tag>& transformable) const
	{
		(*this)(transformable.surface1);
		(*this)(transformable.surface2);
	}

private:
	const matrix_t& _transformation;
};

auto const transform = [](auto& ctx)
{
	auto& transformable = *boost::fusion::at_c<0>(x3::_attr(ctx));
	const auto transformation = boost::fusion::at_c<1>(x3::_attr(ctx));
	vector_t vector = boost::fusion::at_c<2>(x3::_attr(ctx));
	const matrix_t matrix = transformation(vector);
	const transformation_visitor visitor(matrix);
	boost::apply_visitor(visitor, transformable);
};

const x3::symbols<decltype(translation)*> transformation
{
	{"translate", &translation},
	{"rotate", &rotation},
	{"scale", &scaling}
};

auto const transform_rule_def =
		(variable::get::rule > x3::lit('.') > transformation > x3::lit('(') > vector::rule > x3::lit(')')) [transform]
;

BOOST_SPIRIT_DEFINE(def_rule, include_rule, transform_rule);

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
