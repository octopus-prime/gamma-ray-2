/*
 * matrix.cpp
 *
 *  Created on: 13.10.2015
 *      Author: mike_gresens
 */

#include "matrix.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

using namespace boost::numeric::ublas;

namespace rt {

#ifdef COL_MAJOR
typedef column_major major;
#else
typedef row_major major;
#endif

typedef bounded_matrix<float, 4, 4, major> temp_t;

static temp_t
convert(const matrix_t& matrix)
{
	temp_t result;
	for (size_t i = 0; i < result.size1(); ++i)
		for (size_t j = 0; j < result.size2(); ++j)
			result(i, j) = matrix[i][j];
	return result;
}

static matrix_t
convert(const temp_t& matrix)
{
	matrix_t result;
	for (size_t i = 0; i < matrix.size1(); ++i)
		for (size_t j = 0; j < matrix.size2(); ++j)
			result[i][j] = matrix(i, j);
	return result;
}

matrix_t
operator*(const matrix_t& matrix1, const matrix_t& matrix2)
{
	return convert(prod<temp_t>(convert(matrix1), convert(matrix2)));
}

matrix_t
invert(const matrix_t& matrix)
{
	temp_t temp = convert(matrix);
	temp_t result = identity_matrix<float, major>(4, 4);
	permutation_matrix<size_t, bounded_array<size_t,4>> permutation(4);
	lu_factorize(temp, permutation);
	lu_substitute(temp, permutation, result);
	return convert(result);
}

}

namespace std {
ostream&
operator<<(ostream& s, const rt::matrix_t& m)
{
	return s << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3];
}
}
