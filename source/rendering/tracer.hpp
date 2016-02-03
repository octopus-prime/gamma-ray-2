/*
 * tracer.hpp
 *
 *  Created on: 13.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include "../scene/instance.hpp"
#include "../texture/simplexnoise1234.hpp"
#include <boost/algorithm/clamp.hpp>

namespace rt {
namespace rendering {

constexpr float min = 5e-2f;
constexpr float max = 5e+2f;

float noise3(const vector_t v)
{
	const float r = snoise3(v[X], v[Y], v[Z]);
	return boost::algorithm::clamp((r + 1.5f) / 4, 0.5f, 1);
}

float noise3_(const vector_t v)
{
	const float r = snoise3(v[X], v[Y], v[Z]);
	return r;
}

class tracer_t
{
public:
	tracer_t(const scene::instance_t& scene, hits_t& hits)
	:
		_scene(scene),
		_hits(hits)
	{
	}

	color_t
	operator()(const ray_t& ray, const size_t depth) const
	{
		color_t color = make_color(0.0f);

//		if (depth == 0)
//			return color;

		const hits_t::iterator end = _scene.all_hit(ray, _hits.begin());
		if (_hits.begin() == end)
			return color;

		const hit_t hit = *std::min_element(_hits.begin(), end);
		const texture::instance_t& texture = hit.object->texture();
//		const float noise = noise3(hit.point * 10);
//		const float noise_ = noise3_(hit.point * 10);
		const vector_t point = hit.point;// + hit.normal * noise;
		const vector_t normal = hit.normal;// normalize(hit.normal + vector_t{noise,noise,noise} / 10);
//		const vector_t normal = normalize(hit.normal + vector_t{noise_,noise_,noise_} / 8);

		color += texture.ambient;
		for (const auto& light : _scene.lights())
		{
			const vector_t delta = light.source - point;
			const float l = length(delta);
			const vector_t L = delta / l;
			if (depth > 0 && _scene.any_hit({point, L, min, l - min}, _hits.begin()))
				continue;
			const vector_t H = normalize(L - ray.direction);
			const float LN = dot(L, normal);
			const float HN = dot(H, normal);
			const color_t diffuse = std::max(LN, 0.f) * texture.diffuse;// * noise;
			const color_t specular = std::pow(std::max(HN, 0.f), texture.shininess) * texture.specular;
			color += (diffuse + specular) * light.color / (l * l);
		}

		if (depth > 0 && texture.reflection)
		{
			const vector_t R = ray.direction - 2.0f * dot(ray.direction, normal) * normal;
			color += (*this)({point, R, min, max}, depth - 1) * texture.reflection;
		}

		return color;
	}
/*
	color_t
	operator()(const ray_t& ray, const size_t depth) const
	{
		color_t color = make_color(0.0f);

//		if (depth == 0)
//			return color;

		const hits_t::iterator end = _scene.all_hit(ray, _hits.begin());
		if (_hits.begin() == end)
			return color;

		const hit_t hit = *std::min_element(_hits.begin(), end);
		const texture::instance_t& texture = hit.object->texture();
		const float noise = noise3(hit.point);
		const vector_t normal = normalize(hit.normal + noise);

		color += texture.ambient;
		for (const auto& light : _scene.lights())
		{
			const vector_t delta = light.source - hit.point;
			const float l = length(delta);
			const vector_t L = delta / l;
			if (depth > 0 && _scene.any_hit({hit.point, L, min, l - min}, _hits.begin()))
				continue;
			const vector_t H = normalize(L - ray.direction);
			const float LN = dot(L, hit.normal);
			const float HN = dot(H, hit.normal);
			const color_t diffuse = std::max(LN, 0.f) * texture.diffuse * noise;
			const color_t specular = std::pow(std::max(HN, 0.f), texture.shininess) * texture.specular;
			color += (diffuse + specular) * light.color / (l * l);
		}

		if (depth > 0 && texture.reflection)
		{
			const vector_t R = ray.direction - 2.0f * dot(ray.direction, hit.normal) * hit.normal;
			color += (*this)({hit.point, R, min, max}, depth - 1) * texture.reflection;
		}

		return color;
	}
*/
private:
	const scene::instance_t& _scene;
	hits_t& _hits;
};

}
}
