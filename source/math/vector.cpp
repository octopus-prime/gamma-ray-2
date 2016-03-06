/*
 * vector.cpp
 *
 *  Created on: 13.10.2015
 *      Author: mike_gresens
 */

#include "vector.hpp"

namespace rt {

//vector_t
//cross(const vector_t v1, const vector_t v2)
//{
//	return vector_t
//	{
//		v1[1] * v2[2] - v1[2] * v2[1],
//		v1[2] * v2[0] - v1[0] * v2[2],
//		v1[0] * v2[1] - v1[1] * v2[0],
//		0
//	};
//}

}

namespace std {
ostream&
operator<<(ostream& s, const rt::vector_t& v)
{
	return s << v[0] << "," << v[1] << "," << v[2] << "," << v[3];
}
}
