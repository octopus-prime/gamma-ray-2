/*
 * solve.hpp
 *
 *  Created on: 21.12.2015
 *      Author: mike_gresens
 */

#pragma once

#include <gsl/gsl_complex.h>
#include <stddef.h>

int
gsl_poly_solve_quadratic (float a, float b, float c,
		                  float& x0, float& x1);

int
gsl_poly_solve_cubic (float a, float b, float c,
                      float& x0, float& x1, float& x2);

int
gsl_poly_solve_quartic (float a, float b, float c, float d,
						float& x0, float& x1, float& x2, float& x3);

typedef struct
{
  size_t nc ;
  float * matrix ;
}
gsl_poly_complex_workspace_float ;

//gsl_poly_complex_workspace * gsl_poly_complex_workspace_alloc (size_t n);
//void gsl_poly_complex_workspace_free (gsl_poly_complex_workspace * w);

int
gsl_poly_complex_solve (const float * a, size_t n,
                        gsl_poly_complex_workspace_float * w,
                        gsl_complex_packed_float_ptr z);
