/*
 * reader.hpp
 *
 *  Created on: 02.03.2015
 *      Author: mike_gresens
 */

#pragma once

#include "model_fwd.hpp"
#include "rply.h"
#include <memory>

namespace rt {
namespace surface {
namespace primitive {
namespace mesh {

class reader_t
{
public:
	reader_t(const std::shared_ptr<t_ply_> ply);
	void operator()(vertexes_t& vertexes, faces_t& faces) const;

protected:
	static int vertex_cb(p_ply_argument argument);
	static int face_cb(p_ply_argument argument);

private:
	std::shared_ptr<t_ply_> _ply;
};

reader_t
open(const std::string& file);

}
}
}
}
