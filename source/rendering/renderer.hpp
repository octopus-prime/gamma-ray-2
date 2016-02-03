/*
 * renderer.hpp
 *
 *  Created on: 13.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../scene/instance.hpp"
#include "image.hpp"
#include <functional>
#include <atomic>

namespace rt {
namespace rendering {

//template <bool Flag>
//class renderer_t
//{
//public:
//	renderer_t(const size_t width, const size_t height, const size_t depth);
//
//	image_t operator()(const scene::instance_t& scene) const;
//
//private:
//	size_t _width;
//	size_t _height;
//	size_t _depth;
//};

typedef std::function<image_t (const scene::instance_t& scene)> renderer_t;

renderer_t make_renderer(const size_t width, const size_t height, const size_t depth, const bool aa, std::atomic_size_t& done);

}
}
