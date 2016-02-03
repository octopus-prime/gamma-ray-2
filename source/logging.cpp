/*
 * logging.cpp
 *
 *  Created on: 05.03.2015
 *      Author: mike_gresens
 */

#include "logging.hpp"
#include <unordered_map>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters.hpp>
#include <boost/log/support/date_time.hpp>

using namespace boost::log;

namespace rt {

static const std::unordered_map<char, trivial::severity_level>
LEVEL
{
	{'t', trivial::trace},
	{'d', trivial::debug},
	{'i', trivial::info},
	{'w', trivial::warning},
	{'e', trivial::error},
	{'f', trivial::fatal}
};

void
logging(const char level)
{
	core::get()->add_global_attribute("Duration", attributes::timer());
	core::get()->add_global_attribute("Scope", attributes::named_scope());

	const auto formatter = expressions::stream
		<< expressions::format_date_time<attributes::timer::value_type>("Duration", "%H:%M:%S.%f") << " ["
		<< expressions::format_named_scope("Scope", keywords::format = "%n") << "] <"
		<< expressions::attr<trivial::severity_level>("Severity") << "> "
		<< expressions::message;

	const auto logger = add_console_log();

	logger->set_formatter(formatter);
	logger->set_filter(trivial::severity >= LEVEL.at(level));

}

}
