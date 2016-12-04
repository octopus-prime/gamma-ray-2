/*
 * reader.cpp
 *
 *  Created on: 02.03.2015
 *      Author: mike_gresens
 */

#include "reader.hpp"

namespace rt {
namespace surface {
namespace primitive {
namespace mesh {

reader_t::reader_t(const std::shared_ptr<t_ply_> ply)
:
	_ply(ply)
{
}

void
reader_t::operator()(vertexes_t& vertexes, faces_t& faces) const
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
reader_t::vertex_cb(p_ply_argument argument)
{
	long value_index, element_index;
	vertexes_t* vertexes;
	ply_get_argument_element(argument, nullptr, &element_index);
	ply_get_argument_user_data(argument, (void**) &vertexes, &value_index);
	(*vertexes)[element_index][value_index] = ply_get_argument_value(argument);
	return 1;
}

int
reader_t::face_cb(p_ply_argument argument)
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

    return reader_t(ply);
}

}
}
}
}
