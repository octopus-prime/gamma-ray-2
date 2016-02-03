/*
 * engine.cpp
 *
 *  Created on: 12.10.2015
 *      Author: mike_gresens
 */

#include "configuration.hpp"
#include <logging.hpp>
#include "parsing/parser.hpp"
#include "scene/make.hpp"
#include "rendering/renderer.hpp"
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/format.hpp>
#include <future>

static rt::configuration_t
configure(const int argc, const char* const argv[])
{
	const rt::configuration_t configuration = rt::configure(argc, argv);
	rt::logging(configuration.level.at(0));

	BOOST_LOG_NAMED_SCOPE("Configuration");
	BOOST_LOG_TRIVIAL(info) << "Configuration start";
	BOOST_LOG_TRIVIAL(info) << "Input " << '"' << configuration.input << '"';
	BOOST_LOG_TRIVIAL(info) << "Output " << '"' << configuration.output << '"';
	BOOST_LOG_TRIVIAL(info) << "Resolution " << configuration.width << 'x' << configuration.height;
	BOOST_LOG_TRIVIAL(info) << "Recursion " << configuration.depth;
	BOOST_LOG_TRIVIAL(info) << "Antialiasing " << (configuration.aa ? "on" : "off");
	BOOST_LOG_TRIVIAL(info) << "Configuration done" << std::endl;

	return std::move(configuration);
}

static rt::scene::instance_t
preprocess(const rt::configuration_t& configuration)
{
	BOOST_LOG_NAMED_SCOPE("Preprocessing");
	BOOST_LOG_TRIVIAL(info) << "Preprocessing start";

	const rt::scene::description_t description = rt::parsing::parse(configuration.input);
	const rt::scene::instance_t instance = rt::scene::make(description);

	BOOST_LOG_TRIVIAL(info) << "Created " << instance.lights().size() << " lights";
	BOOST_LOG_TRIVIAL(info) << "Created " << instance.objects().size() << " objects";
	BOOST_LOG_TRIVIAL(info) << "Preprocessing done" << std::endl;

	return std::move(instance);
}

static rt::rendering::image_t
render(const rt::configuration_t& configuration, const rt::scene::instance_t& scene)
{
	BOOST_LOG_NAMED_SCOPE("Rendering");
	BOOST_LOG_TRIVIAL(info) << "Rendering start";

	std::atomic_size_t done(0);
	const float factor = 100.f / (configuration.width * configuration.height);
	const rt::rendering::renderer_t render = rt::rendering::make_renderer(configuration.width, configuration.height, configuration.depth, configuration.aa, done);
	std::future<rt::rendering::image_t> future = std::async(std::launch::async, render, scene);

	for (;;)
	{
		const std::future_status status = future.wait_for(std::chrono::seconds(5));
		if (status == std::future_status::ready)
			break;
		const float percent = done * factor;
		BOOST_LOG_TRIVIAL(info) << boost::format("%6.2f") % percent << "% done";
	}

	const rt::rendering::image_t image = future.get();

	BOOST_LOG_TRIVIAL(info) << "Rendering done" << std::endl;

	return std::move(image);
}

static void
postprocess(const rt::rendering::writer_t& write, const rt::rendering::image_t& image)
{
	BOOST_LOG_NAMED_SCOPE("Postprocessing");
	BOOST_LOG_TRIVIAL(info) << "Postprocessing start";

	write(image);

	BOOST_LOG_TRIVIAL(info) << "Postprocessing done" << std::endl;
}

int
main(int argc, char** argv)
{
	std::cout << std::endl << "γ-ray version 0.2" << std::endl << std::endl;

	try
	{
		const rt::configuration_t configuration = configure(argc, argv);
		const rt::rendering::writer_t write = rt::rendering::make_writer(configuration.output);
		const rt::scene::instance_t scene = preprocess(configuration);
		const rt::rendering::image_t image = render(configuration, scene);
		postprocess(write, image);
		return EXIT_SUCCESS;
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Error: " << exception.what() << std::endl;
		return EXIT_FAILURE;
	}
}
