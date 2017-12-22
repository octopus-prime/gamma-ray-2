/*
 * reader.cpp
 *
 *  Created on: 02.03.2015
 *      Author: mike_gresens
 */

#include "reader.hpp"
#include "rply.h"
#include <memory>
#include <unordered_map>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/functional/hash.hpp>

namespace fs = boost::filesystem;

namespace rt {
namespace surface {
namespace primitive {
namespace mesh {
namespace ply {

class ply_reader_t
{
public:
	ply_reader_t(const std::shared_ptr<t_ply_> ply);
	void operator()(vertexes_t& vertexes, faces_t& faces) const;

protected:
	static int vertex_cb(p_ply_argument argument);
	static int face_cb(p_ply_argument argument);

private:
	std::shared_ptr<t_ply_> _ply;
};

ply_reader_t::ply_reader_t(const std::shared_ptr<t_ply_> ply)
:
	_ply(ply)
{
}

void
ply_reader_t::operator()(vertexes_t& vertexes, faces_t& faces) const
{
	const long nvertexes = ply_set_read_cb(_ply.get(), "vertex", "x", vertex_cb, &vertexes, 0);
	ply_set_read_cb(_ply.get(), "vertex", "y", vertex_cb, &vertexes, 1);
	ply_set_read_cb(_ply.get(), "vertex", "z", vertex_cb, &vertexes, 2);
	const long ntriangles = ply_set_read_cb(_ply.get(), "face", "vertex_indices", face_cb, &faces, 0);

	vertexes.resize(nvertexes);
	faces.resize(ntriangles);

	if (!ply_read(_ply.get()))
		throw std::runtime_error("Can't read.");
}

int
ply_reader_t::vertex_cb(p_ply_argument argument)
{
	long value_index, element_index;
	vertexes_t* vertexes;
	ply_get_argument_element(argument, nullptr, &element_index);
	ply_get_argument_user_data(argument, (void**) &vertexes, &value_index);
	(*vertexes)[element_index][value_index] = ply_get_argument_value(argument);
	return 1;
}

int
ply_reader_t::face_cb(p_ply_argument argument)
{
	long value_index, element_index;
	faces_t* faces;
	ply_get_argument_element(argument, nullptr, &element_index);
	ply_get_argument_property(argument, nullptr, nullptr, &value_index);
	ply_get_argument_user_data(argument, (void**) &faces, nullptr);
	if (value_index != -1)
		(*faces)[element_index][value_index] = ply_get_argument_value(argument);
	return 1;
}

reader_t
open(const std::string& file)
{
	const std::shared_ptr<t_ply_> ply
	(
		ply_open(file.c_str(), nullptr, 0, nullptr),
		ply_close
	);

    if (!ply)
    	throw std::runtime_error("Can't open file: " + file);

    if (!ply_read_header(ply.get()))
    	throw std::runtime_error("Can't read header: " + file);

    return ply_reader_t(ply);
}

}
namespace stl {

using triangle_t = std::array<vector_t, 3>;
using triangles_t = std::vector<triangle_t>;

vector_t read_vector(const float* const data, float const w) {
	return vector_t {data[0], data[1], data[2], w};
}

triangles_t read_mesh(const std::string& file) {
	std::ifstream stream(file);
	char buffer[84];
	stream.read(buffer, 84);
	size_t size = *reinterpret_cast<std::uint32_t*>(buffer + 80);
	triangles_t triangles;
	triangles.reserve(size);
	for (size_t i = 0; i < size; ++i) {
		stream.read(buffer, 50);
		triangles.push_back(triangle_t {
			read_vector(reinterpret_cast<float*>(buffer) + 3, 1),
			read_vector(reinterpret_cast<float*>(buffer) + 6, 1),
			read_vector(reinterpret_cast<float*>(buffer) + 9, 1)
		});
	}
	return triangles;
}

struct hash {
	size_t operator()(const vector_t vector) const noexcept {
		__v2du const value = (__v2du) vector;
		size_t seed = 0;
		boost::hash_combine(seed, value[0]);
		boost::hash_combine(seed, value[1]);
		return seed;
	}
};

struct equal {
	bool operator()(const vector_t vector1, const vector_t vector2) const noexcept {
	    return _mm_movemask_ps(_mm_cmpeq_ps(vector1, vector2)) == 0b1111;
	}
};

class sdl_reader_t {
public:
	sdl_reader_t(triangles_t&& triangles) : triangles(std::forward<triangles_t>(triangles)) {}
	void operator()(vertexes_t& vertexes, faces_t& faces) const {
		std::unordered_map<vector_t, std::uint32_t, hash, equal> map;
		faces.reserve(triangles.size());
		for (auto const& triangle : triangles) {
			face_t face;
			for (size_t i = 0; i < 3; ++i) {
				auto const& vertex = triangle[i];
				auto const foo = map.emplace(vertex, vertexes.size());
				if (foo.second)
					vertexes.push_back(foo.first->first);
				face[i] = foo.first->second;
			}
			faces.push_back(face);
		}
	}
private:
	triangles_t triangles;
};

reader_t
open(const std::string& file)
{
	triangles_t triangles = read_mesh(file);
	return sdl_reader_t(std::move(triangles));
}

}

reader_t
open(const std::string& file)
{
	const fs::path path(fs::absolute(file));
	const std::string extension = path.extension().string();

	static const std::unordered_map<std::string, std::function<reader_t (const std::string&)>> readers
	{
		{".ply", ply::open},
		{".stl", stl::open},
	};

	return readers.at(extension)(file);
}

}
}
}
}
