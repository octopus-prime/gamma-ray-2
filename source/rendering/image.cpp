/*
 * image.cpp
 *
 *  Created on: 01.11.2015
 *      Author: mike_gresens
 */

// Hack for Mac :(
// Should be part of boost::gil !!
#ifndef _WIN32
#define png_infopp_NULL nullptr
#define int_p_NULL nullptr
#define png_bytep_NULL nullptr
#endif

#include "image.hpp"
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/extension/io/tiff_io.hpp>
#include <boost/filesystem.hpp>
#include <unordered_map>
#include <iostream>

namespace fs = boost::filesystem;
namespace gil = boost::gil;

namespace rt {
namespace rendering {

static void
write_jpg(const image_t& image, const fs::path& file)
{
	const auto view = gil::color_converted_view<gil::rgb8_pixel_t>(gil::const_view(image));
	gil::jpeg_write_view(file.string(), view);
}

static void
write_png(const image_t& image, const fs::path& file)
{
	const auto view = gil::color_converted_view<gil::rgb16_pixel_t>(gil::const_view(image));
	gil::png_write_view(file.string(), view);
}

static void
write_tif(const image_t& image, const fs::path& file)
{
	const auto view = gil::const_view(image);
	gil::tiff_write_view(file.string(), view);
}

writer_t
make_writer(const std::string& output)
{
	const fs::path file(fs::absolute(output));
	fs::create_directories(file.parent_path());

	static const std::unordered_map<std::string, writer_t> writers
	{
		{".jpg", std::bind(write_jpg, std::placeholders::_1, file)},
		{".png", std::bind(write_png, std::placeholders::_1, file)},
		{".tif", std::bind(write_tif, std::placeholders::_1, file)}
	};

	const std::string extension = file.extension().string();

	try
	{
		return writers.at(extension);
	}
	catch (const std::out_of_range& exception)
	{
		throw std::runtime_error("Extension '" + extension + "' not valid. Use '.jpg', '.png' or '.tif'.");
	}
}

}
}
