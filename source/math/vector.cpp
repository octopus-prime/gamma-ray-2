/*
 * vector.cpp
 *
 *  Created on: 13.10.2015
 *      Author: mike_gresens
 */

#include "vector.hpp"

namespace std {

ostream&
operator<<(ostream& s, const rt::vector_t v)
{
	return s << '<' << v[0] << "," << v[1] << "," << v[2] << "," << v[3] << '>';
}

}
