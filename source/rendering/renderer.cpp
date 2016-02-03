/*
 * renderer.cpp
 *
 *  Created on: 13.12.2015
 *      Author: mike_gresens
 */

#include "renderer.hpp"
#include "tracer.hpp"
#include <tbb/enumerable_thread_specific.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range2d.h>
//#include <chrono>
//
//using namespace std::chrono;

namespace rt {
namespace rendering {

constexpr std::array<std::array<float, 2>, 4> RGSS4
{{
	{{-0.4f, +0.1f}},
	{{+0.1f, +0.4f}},
	{{+0.4f, -0.1f}},
	{{-0.1f, -0.4f}}
}};

constexpr std::array<std::array<float, 2>, 9> RGSS9
{{
	{{-0.4f, +0.1f}},
	{{+0.1f, +0.4f}},
	{{+0.4f, -0.1f}},
	{{-0.1f, -0.4f}},
	{{+0.0f, +0.0f}},
	{{-0.3f, +0.2f}},
	{{+0.2f, +0.3f}},
	{{+0.3f, -0.2f}},
	{{-0.2f, -0.3f}}
}};

//template <bool Flag>
//renderer_t<Flag>::renderer_t(const size_t width, const size_t height, const size_t depth)
//:
//	_width(width),
//	_height(height),
//	_depth(depth)
//{
//}
//
//template <>
//image_t
//renderer_t<true>::operator()(const scene::instance_t& scene) const
//{
//	const float factor = float(_width) / float(_height);
//	const float ix = 1.0f / _width;
//	const float iy = 1.0f / _height;
//
//	const scene::camera::instance_t& camera = scene.camera();
//
//	image_t image(_width, _height);
//	tbb::enumerable_thread_specific<hits_t> hits(20);
//	const auto t0 = system_clock::now();
//	tbb::parallel_for
//	(
//		tbb::blocked_range2d<size_t, size_t>(0, _height, 0, _width),
//		[&](const tbb::blocked_range2d<size_t, size_t>& range)
//		{
//			const tracer_t trace(scene, hits.local());
//			for (size_t h = range.rows().begin(); h < range.rows().end(); ++h)
//			{
//				const float y = float(h) * iy - 0.5f;
//				for (size_t w = range.cols().begin(); w < range.cols().end(); ++w)
//				{
//					const float x = float(w) * ix - 0.5f;
//					color_t color {0,0,0,0};
//					for (const auto& delta : deltas)
//					{
//						const float X = x + ix * delta[0];
//						const float Y = y + iy * delta[1];
//						const vector_t direction = normalize(camera.direction + camera.right * X * factor + camera.up * Y);
//						const ray_t ray {camera.origin, direction, min, max};
//						color += trace(ray, _depth);
//					}
//					flipped_up_down_view(flipped_left_right_view(view(image)))(w, h) = to_pixel(color * 0.25f);
//				}
//			}
//		}
//	);
//	const auto t1 = system_clock::now();
//
//	std::cout << "t = " << duration_cast<milliseconds>(t1 - t0).count() << " ms" << std::endl;
//
//	return std::move(image);
//}
//
//template <>
//image_t
//renderer_t<false>::operator()(const scene::instance_t& scene) const
//{
//	const float factor = float(_width) / float(_height);
//	const float ix = 1.0f / _width;
//	const float iy = 1.0f / _height;
//
//	const scene::camera::instance_t& camera = scene.camera();
//
//	image_t image(_width, _height);
//	tbb::enumerable_thread_specific<hits_t> hits(20);
//	const auto t0 = system_clock::now();
//	tbb::parallel_for
//	(
//		tbb::blocked_range2d<size_t, size_t>(0, _height, 0, _width),
//		[&](const tbb::blocked_range2d<size_t, size_t>& range)
//		{
//			const tracer_t trace(scene, hits.local());
//			for (size_t h = range.rows().begin(); h < range.rows().end(); ++h)
//			{
//				const float y = float(h) * iy - 0.5f;
//				for (size_t w = range.cols().begin(); w < range.cols().end(); ++w)
//				{
//					const float x = float(w) * ix - 0.5f;
//					const vector_t direction = normalize(camera.direction + camera.right * x * factor + camera.up * y);
//					const ray_t ray {camera.origin, direction, min, max};
//					const color_t color = trace(ray, _depth);
//					flipped_up_down_view(flipped_left_right_view(view(image)))(w, h) = to_pixel(color);
//				}
//			}
//		}
//	);
//	const auto t1 = system_clock::now();
//
//	std::cout << "t = " << duration_cast<milliseconds>(t1 - t0).count() << " ms" << std::endl;
//
//	return std::move(image);
//}
//
//template class renderer_t<true>;
//template class renderer_t<false>;

renderer_t make_renderer(const size_t width, const size_t height, const size_t depth, const bool aa, std::atomic_size_t& done)
{
	if (aa)
		return [width, height, depth, &done](const scene::instance_t& scene)
		{
			const float factor = float(width) / float(height);
			const float ix = 1.0f / width;
			const float iy = 1.0f / height;

			const scene::camera::instance_t& camera = scene.camera();

			image_t image(width, height);
			tbb::enumerable_thread_specific<hits_t> hits(1000);
//			const auto t0 = system_clock::now();
			tbb::parallel_for
			(
				tbb::blocked_range2d<size_t, size_t>(0, height, 0, width),
				[&](const tbb::blocked_range2d<size_t, size_t>& range)
				{
					const tracer_t trace(scene, hits.local());
					for (size_t h = range.rows().begin(); h < range.rows().end(); ++h)
					{
						const float y = float(h) * iy - 0.5f;
						for (size_t w = range.cols().begin(); w < range.cols().end(); ++w)
						{
							const float x = float(w) * ix - 0.5f;
							color_t color {0,0,0,0};
							for (const auto& delta : RGSS4)
							{
								const float X = x + ix * delta[0];
								const float Y = y + iy * delta[1];
								const vector_t direction = normalize(camera.direction + camera.right * X * factor + camera.up * Y);
//								const vector_t direction = camera.direction + camera.right * X * factor + camera.up * Y;
								const ray_t ray {camera.origin, direction, min, max};
								color += trace(ray, depth);
							}
							flipped_up_down_view(flipped_left_right_view(view(image)))(w, h) = to_pixel(color / float(RGSS4.size()));
						}
					}
					done += range.rows().size() * range.cols().size();
				}
			);
//			const auto t1 = system_clock::now();
//
//			std::cout << "t = " << duration_cast<milliseconds>(t1 - t0).count() << " ms" << std::endl;

			return std::move(image);
		};
	else
		return [width, height, depth, &done](const scene::instance_t& scene)
		{
			const float factor = float(width) / float(height);
			const float ix = 1.0f / width;
			const float iy = 1.0f / height;

			const scene::camera::instance_t& camera = scene.camera();

			image_t image(width, height);
			tbb::enumerable_thread_specific<hits_t> hits(1000);
//			const auto t0 = system_clock::now();
			tbb::parallel_for
			(
				tbb::blocked_range2d<size_t, size_t>(0, height, 0, width),
				[&](const tbb::blocked_range2d<size_t, size_t>& range)
				{
					const tracer_t trace(scene, hits.local());
					for (size_t h = range.rows().begin(); h < range.rows().end(); ++h)
					{
						const float y = float(h) * iy - 0.5f;
						for (size_t w = range.cols().begin(); w < range.cols().end(); ++w)
						{
							const float x = float(w) * ix - 0.5f;
							const vector_t direction = normalize(camera.direction + camera.right * x * factor + camera.up * y);
							const ray_t ray {camera.origin, direction, min, max};
							const color_t color = trace(ray, depth);
							flipped_up_down_view(flipped_left_right_view(view(image)))(w, h) = to_pixel(color);
						}
					}
					done += range.rows().size() * range.cols().size();
				}
			);
//			const auto t1 = system_clock::now();
//
//			std::cout << "t = " << duration_cast<milliseconds>(t1 - t0).count() << " ms" << std::endl;

			return std::move(image);
		};
}

}
}
