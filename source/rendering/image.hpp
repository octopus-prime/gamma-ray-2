/*
 * image.hpp
 *
 *  Created on: 01.11.2015
 *      Author: mike_gresens
 */

#pragma once

#include "color.hpp"
#include "../math/fmath.hpp"
#include <boost/gil/typedefs.hpp>
#include <boost/gil/image.hpp>
#include <functional>

using namespace boost::gil;

namespace rt {
namespace rendering {

typedef rgb32f_image_t image_t;
typedef rgb32f_pixel_t pixel_t;

inline pixel_t
to_pixel(const color_t color)
{
	const color_t temp = color_t {1,1,1,0} - fmath::exp_ps(-color);
	return pixel_t(temp[0], temp[1], temp[2]);
}

typedef std::function<void (image_t)> writer_t;

writer_t
make_writer(const std::string& output);

}
}
