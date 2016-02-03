/*
 * solve.cpp
 *
 *  Created on: 21.12.2015
 *      Author: mike_gresens
 */

#include "solve.hpp"
#include <gsl/gsl_poly.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <cmath>
#include <algorithm>

int
gsl_poly_solve_quadratic (float a, float b, float c, float *x0, float *x1)
{
  if (a == 0.0f) /* Handle linear case */
	{
	  if (b == 0.0f)
		{
		  return 0;
		}
	  else
		{
		  *x0 = -c / b;
		  return 1;
		};
	}

  {
	  float disc = b * b - 4.0f * a * c;

	if (disc > 0.0f)
	  {
		if (b == 0.0f)
		  {
			float r = std::sqrt (-c / a);
			*x0 = -r;
			*x1 =  r;
		  }
		else
		  {
//			float sgnb = (b > 0.0f ? 1.0f : -1.0f);
//			float temp = -0.5f * (b + sgnb * std::sqrt (disc));
			float temp = -0.5f * (b + std::copysign(std::sqrt (disc), b));
			float r1 = temp / a ;
			float r2 = c / temp ;

			if (r1 < r2)
			  {
				*x0 = r1 ;
				*x1 = r2 ;
			  }
			else
			  {
				*x0 = r2 ;
				*x1 = r1 ;
			  }
		  }
		return 2;
	  }
	else if (disc == 0.0f)
	  {
		*x0 = *x1 = -0.5f * b / a ;
		return 2 ;
	  }
	else
	  {
		return 0;
	  }
  }
}

int
gsl_poly_solve_cubic (float a, float b, float c,
                      float *x0, float *x1, float *x2)
{
  float q = (a * a - 3 * b);
  float r = (2 * a * a * a - 9 * a * b + 27 * c);

  float Q = q / 9;
  float R = r / 54;

  float Q3 = Q * Q * Q;
  float R2 = R * R;

  float CR2 = 729 * r * r;
  float CQ3 = 2916 * q * q * q;

  if (R == 0 && Q == 0)
    {
      *x0 = - a / 3 ;
      *x1 = - a / 3 ;
      *x2 = - a / 3 ;
      return 3 ;
    }
  else if (CR2 == CQ3)
    {
      /* this test is actually R2 == Q3, written in a form suitable
         for exact computation with integers */

      /* Due to finite precision some float roots may be missed, and
         considered to be a pair of complex roots z = x +/- epsilon i
         close to the real axis. */

      float sqrtQ = std::sqrt (Q);

      if (R > 0)
        {
          *x0 = -2 * sqrtQ  - a / 3;
          *x1 = sqrtQ - a / 3;
          *x2 = sqrtQ - a / 3;
        }
      else
        {
          *x0 = - sqrtQ  - a / 3;
          *x1 = - sqrtQ - a / 3;
          *x2 = 2 * sqrtQ - a / 3;
        }
      return 3 ;
    }
  else if (R2 < Q3)
    {
      float sgnR = (R >= 0 ? 1 : -1);
      float ratio = sgnR * std::sqrt (R2 / Q3);
//      float ratio = std::copysign(std::sqrt (R2 / Q3), R);
      float theta = std::acos (ratio);
      float norm = -2 * std::sqrt (Q);
      *x0 = norm * std::cos (theta / 3) - a / 3;
      *x1 = norm * std::cos ((theta + 2 * M_PI) / 3) - a / 3;
      *x2 = norm * std::cos ((theta - 2 * M_PI) / 3) - a / 3;

      /* Sort *x0, *x1, *x2 into increasing order */

      if (*x0 > *x1)
        std::swap(*x0, *x1) ;

      if (*x1 > *x2)
        {
          std::swap(*x1, *x2) ;

          if (*x0 > *x1)
            std::swap(*x0, *x1) ;
        }

      return 3;
    }
  else
    {
      float sgnR = (R >= 0 ? 1 : -1);
      float A = -sgnR * std::cbrt (std::fabs (R) + std::sqrt (R2 - Q3) );
//      float A = -std::copysign(std::cbrt (std::fabs (R) + std::sqrt (R2 - Q3) ), R);
      float B = Q / A ;
      *x0 = A + B - a / 3;
      return 1;
    }
}

int
gsl_poly_solve_quartic (float a, float b, float c, float d,
						float *x0, float *x1, float *x2, float *x3)
{
  /*
   * This code is based on a simplification of
   * the algorithm from zsolve_quartic.c for real roots
   */
  float u[3];
  float aa, pp, qq, rr, rc, sc, tc, mt;
  float w1r, w1i, w2r, w2i, w3r;
  float v[3], v1, v2, arg, theta;
  float disc, h;
  int k1 = 0, k2 = 0;
  float zarr[4];

  /* Deal easily with the cases where the quartic is degenerate. The
   * ordering of solutions is done explicitly. */
  if (0.0f == b && 0.0f == c)
	{
	  if (0.0f == d)
		{
		  if (a > 0.0f)
			{
			  *x0 = -a;
			  *x1 = 0.0f;
			  *x2 = 0.0f;
			  *x3 = 0.0f;
			}
		  else
			{
			  *x0 = 0.0f;
			  *x1 = 0.0f;
			  *x2 = 0.0f;
			  *x3 = -a;
			}
		  return 4;
		}
	  else if (0.0f == a)
		{
		  if (d > 0.0f)
			{
			  return 0;
			}
		  else
			{
			  *x1 = sqrtf (sqrtf (-d));
			  *x0 = -(*x1);
			  return 2;
			}
		}
	}

  if (0.0f == c && 0.0f == d)
	{
	  *x0=0.0f;
	  *x1=0.0f;
	  if (gsl_poly_solve_quadratic(1.0f,a,b,x2,x3)==0) {
	mt=3;
	  } else {
	mt=1;
	  }
	}
  else
	{
	  /* For non-degenerate solutions, proceed by constructing and
	   * solving the resolvent cubic */
	  aa = a * a;
	  pp = b - (3.0f/8.0f) * aa;
	  qq = c - (1.0f/2.0f) * a * (b - (1.0f/4.0f) * aa);
	  rr = d - (1.0f/4.0f) * (a * c - (1.0f/4.0f) * aa * (b - (3.0f/16.0f) * aa));
	  rc = (1.0f/2.0f) * pp;
	  sc = (1.0f/4.0f) * ((1.0f/4.0f) * pp * pp - rr);
	  tc = -((1.0f/8.0f) * qq * (1.0f/8.0f) * qq);

	  /* This code solves the resolvent cubic in a convenient fashion
	   * for this implementation of the quartic. If there are three real
	   * roots, then they are placed directly into u[].  If two are
	   * complex, then the real root is put into u[0] and the real
	   * and imaginary part of the complex roots are placed into
	   * u[1] and u[2], respectively. Additionally, this
	   * calculates the discriminant of the cubic and puts it into the
	   * variable disc. */
	  {
	float qcub = (rc * rc - 3.f * sc);
	float rcub = (2.f * rc * rc * rc - 9.f * rc * sc + 27.f * tc);

	float Q = qcub / 9.f;
	float R = rcub / 54.f;

	float Q3 = Q * Q * Q;
	float R2 = R * R;

	float CR2 = 729.f * rcub * rcub;
	float CQ3 = 2916.f * qcub * qcub * qcub;

	disc = (CR2 - CQ3) / 2125764.0f;

	const float rc3 = rc / 3.f;

	if (0 == R && 0 == Q)
	  {
		u[0] = -rc3;
		u[1] = -rc3;
		u[2] = -rc3;
	  }
	else if (CR2 == CQ3)
	  {
		float sqrtQ = sqrtf (Q);
		if (R > 0)
		  {
		u[0] = -2.f * sqrtQ - rc3;
		u[1] = sqrtQ - rc3;
		u[2] = sqrtQ - rc3;
		  }
		else
		  {
		u[0] = -sqrtQ - rc3;
		u[1] = -sqrtQ - rc3;
		u[2] = 2.f * sqrtQ - rc3;
		  }
	  }
	else if (CR2 < CQ3)
	  {
		float sqrtQ = sqrtf (Q);
		float sqrtQ3 = sqrtQ * sqrtQ * sqrtQ;
		float theta = acosf (R / sqrtQ3);
		if (R / sqrtQ3 >= 1.0f) theta = 0.0f;
		{
		  float norm = -2.f * sqrtQ;

		  u[0] = norm * cosf (theta / 3.f) - rc3;
		  u[1] = norm * cosf ((theta + 2.0f * M_PI) / 3.f) - rc3;
		  u[2] = norm * cosf ((theta - 2.0f * M_PI) / 3.f) - rc3;
		}
	  }
	else
	  {
//		float sgnR = (R >= 0 ? 1.f : -1.f);
//		float modR = fabsf (R);
		float sqrt_disc = sqrtf (R2 - Q3);
//		float A = -sgnR * std::cbrt (modR + sqrt_disc);
		float A = -std::copysign(std::cbrt (std::abs(R) + sqrt_disc), R);
		float B = Q / A;
		float mod_diffAB = fabsf (A - B);

		u[0] = A + B - rc3;
		u[1] = -0.5f * (A + B) - rc3;
		u[2] = -(sqrtf(3.0f) / 2.0f) * mod_diffAB;
	  }
	  }
	  /* End of solution to resolvent cubic */

	  /* Combine the square roots of the roots of the cubic
	   * resolvent appropriately. Also, calculate 'mt' which
	   * designates the nature of the roots:
	   * mt=1 : 4 real roots (disc == 0)
	   * mt=2 : 0 real roots (disc < 0)
	   * mt=3 : 2 real roots (disc > 0)
	   */

	  if (0.0f == disc)
	u[2] = u[1];

	  if (0 >= disc)
	{
	  mt = 2;

	  /* One would think that we could return 0 here and exit,
	   * since mt=2. However, this assignment is temporary and
	   * changes to mt=1 under certain conditions below.
	   */

	  v[0] = fabsf (u[0]);
	  v[1] = fabsf (u[1]);
	  v[2] = fabsf (u[2]);

	  v1 = std::max({v[0], v[1], v[2]});
	  /* Work out which two roots have the largest moduli */
	  k1 = 0, k2 = 0;
	  if (v1 == v[0])
		{
		  k1 = 0;
		  v2 = fmaxf (v[1], v[2]);
		}
	  else if (v1 == v[1])
		{
		  k1 = 1;
		  v2 = fmaxf (v[0], v[2]);
		}
	  else
		{
		  k1 = 2;
		  v2 = fmaxf (v[0], v[1]);
		}

	  if (v2 == v[0])
		{
		  k2 = 0;
		}
	  else if (v2 == v[1])
		{
		  k2 = 1;
		}
	  else
		{
		  k2 = 2;
		}

	  if (0.0f <= u[k1])
		{
		  w1r=sqrtf(u[k1]);
		  w1i=0.0f;
		}
	  else
		{
		  w1r=0.0f;
		  w1i=sqrtf(-u[k1]);
		}
	  if (0.0f <= u[k2])
		{
		  w2r=sqrtf(u[k2]);
		  w2i=0.0f;
		}
	  else
		{
		  w2r=0.0f;
		  w2i=sqrtf(-u[k2]);
		}
	}
	  else
	{
	  mt = 3;

	  if (0.0f == u[1] && 0.0f == u[2])
		{
		  arg = 0.0f;
		}
	  else
		{
		  arg = sqrtf(sqrtf(u[1] * u[1] + u[2] * u[2]));
		}
	  theta = atan2f(u[2], u[1]);

	  sincosf(theta / 2.0f, &w1i, &w1r);
//	  w1r = arg * cosf(theta / 2.0f);
//	  w1i = arg * sinf(theta / 2.0f);
	  w1r *= arg;
	  w1i *= arg;
	  w2r = w1r;
	  w2i = -w1i;
	}

	  /* Solve the quadratic to obtain the roots to the quartic */
	  w3r = qq / 8.0f * (w1i * w2i - w1r * w2r) /
	(w1i * w1i + w1r * w1r) / (w2i * w2i + w2r * w2r);
	  h = a / 4.0f;

	  zarr[0] = w1r + w2r + w3r - h;
	  zarr[1] = -w1r - w2r + w3r - h;
	  zarr[2] = -w1r + w2r - w3r - h;
	  zarr[3] = w1r - w2r - w3r - h;

	  /* Arrange the roots into the variables z0, z1, z2, z3 */
	  if (2 == mt)
		{
		  if (u[k1] >= 0.f && u[k2] >= 0.f)
			{
			  mt = 1;
		  *x0 = zarr[0];
		  *x1 = zarr[1];
		  *x2 = zarr[2];
		  *x3 = zarr[3];
			}
	  else
		{
		  return 0;
		}
	}
	  else
		{
	  *x0 = zarr[0];
	  *x1 = zarr[1];
		}
	}

  /* Sort the roots as usual */
  if (1 == mt)
	{
	  /* Roots are all real, sort them by the real part */
//	  if (*x0 > *x1)
//		  std::swap (*x0, *x1);
//	  if (*x0 > *x2)
//		  std::swap (*x0, *x2);
//	  if (*x0 > *x3)
//		  std::swap (*x0, *x3);
//
//	  if (*x1 > *x2)
//		  std::swap (*x1, *x2);
//	  if (*x2 > *x3)
//		{
//		  std::swap (*x2, *x3);
//		  if (*x1 > *x2)
//			  std::swap (*x1, *x2);
//		}
	  return 4;
	}
  else
	{
	  /* 2 real roots */
//	  if (*x0 > *x1)
//		  std::swap (*x0, *x1);
	}

  return 2;
}

#define MAT(m,i,j,n) ((m)[(i)*(n) + (j)])
#define FMAT(m,i,j,n) ((m)[((i)-1)*(n) + ((j)-1)])

static void
set_companion_matrix (const float *a, size_t nc, float *m)
{
  size_t i, j;

  for (i = 0; i < nc; i++)
    for (j = 0; j < nc; j++)
      MAT (m, i, j, nc) = 0.0;

  for (i = 1; i < nc; i++)
    MAT (m, i, i - 1, nc) = 1.0;

  for (i = 0; i < nc; i++)
    MAT (m, i, nc - 1, nc) = -a[i] / a[nc];
}

#define RADIX 2
#define RADIX2 (RADIX*RADIX)

static void
balance_companion_matrix (float *m, size_t nc)
{
  int not_converged = 1;

  float row_norm = 0;
  float col_norm = 0;

  while (not_converged)
    {
      size_t i, j;
      float g, f, s;

      not_converged = 0;

      for (i = 0; i < nc; i++)
        {
          /* column norm, excluding the diagonal */

          if (i != nc - 1)
            {
              col_norm = fabsf (MAT (m, i + 1, i, nc));
            }
          else
            {
              col_norm = 0;

              for (j = 0; j < nc - 1; j++)
                {
                  col_norm += fabsf (MAT (m, j, nc - 1, nc));
                }
            }

          /* row norm, excluding the diagonal */

          if (i == 0)
            {
              row_norm = fabsf (MAT (m, 0, nc - 1, nc));
            }
          else if (i == nc - 1)
            {
              row_norm = fabsf (MAT (m, i, i - 1, nc));
            }
          else
            {
              row_norm = (fabsf (MAT (m, i, i - 1, nc))
                          + fabsf (MAT (m, i, nc - 1, nc)));
            }

          if (col_norm == 0 || row_norm == 0)
            {
              continue;
            }

          g = row_norm / RADIX;
          f = 1;
          s = col_norm + row_norm;

          while (col_norm < g)
            {
              f *= RADIX;
              col_norm *= RADIX2;
            }

          g = row_norm * RADIX;

          while (col_norm > g)
            {
              f /= RADIX;
              col_norm /= RADIX2;
            }

          if ((row_norm + col_norm) < 0.95 * s * f)
            {
              not_converged = 1;

              g = 1 / f;

              if (i == 0)
                {
                  MAT (m, 0, nc - 1, nc) *= g;
                }
              else
                {
                  MAT (m, i, i - 1, nc) *= g;
                  MAT (m, i, nc - 1, nc) *= g;
                }

              if (i == nc - 1)
                {
                  for (j = 0; j < nc; j++)
                    {
                      MAT (m, j, i, nc) *= f;
                    }
                }
              else
                {
                  MAT (m, i + 1, i, nc) *= f;
                }
            }
        }
    }
}

static int
qr_companion (float *h, size_t nc, gsl_complex_packed_float_ptr zroot)
{
	float t = 0.0;

  size_t iterations, e, i, j, k, m;

  float w, x, y, s, z;

  float p = 0, q = 0, r = 0;

  /* FIXME: if p,q,r, are not set to zero then the compiler complains
     that they ``might be used uninitialized in this
     function''. Looking at the code this does seem possible, so this
     should be checked. */

  int notlast;

  size_t n = nc;

next_root:

  if (n == 0)
    return GSL_SUCCESS ;

  iterations = 0;

next_iteration:

  for (e = n; e >= 2; e--)
    {
      double a1 = fabsf (FMAT (h, e, e - 1, nc));
      double a2 = fabsf (FMAT (h, e - 1, e - 1, nc));
      double a3 = fabsf (FMAT (h, e, e, nc));

      if (a1 <= GSL_FLT_EPSILON * (a2 + a3))
        break;
    }

  x = FMAT (h, n, n, nc);

  if (e == n)
    {
      GSL_SET_COMPLEX_PACKED (zroot, n-1, x + t, 0); /* one real root */
      n--;
      goto next_root;
      /*continue;*/
    }

  y = FMAT (h, n - 1, n - 1, nc);
  w = FMAT (h, n - 1, n, nc) * FMAT (h, n, n - 1, nc);

  if (e == n - 1)
    {
      p = (y - x) / 2;
      q = p * p + w;
      y = sqrtf (fabsf (q));

      x += t;

      if (q > 0)                /* two real roots */
        {
          if (p < 0)
            y = -y;
          y += p;

          GSL_SET_COMPLEX_PACKED (zroot, n-1, x - w / y, 0);
          GSL_SET_COMPLEX_PACKED (zroot, n-2, x + y, 0);
        }
      else
        {
          GSL_SET_COMPLEX_PACKED (zroot, n-1, x + p, -y);
          GSL_SET_COMPLEX_PACKED (zroot, n-2, x + p, y);
        }
      n -= 2;

      goto next_root;
      /*continue;*/
    }

  /* No more roots found yet, do another iteration */

  if (iterations == 120)  /* increased from 30 to 120 */
    {
      /* too many iterations - give up! */

      return GSL_FAILURE ;
    }

  if (iterations % 10 == 0 && iterations > 0)
    {
      /* use an exceptional shift */

      t += x;

      for (i = 1; i <= n; i++)
        {
          FMAT (h, i, i, nc) -= x;
        }

      s = fabsf (FMAT (h, n, n - 1, nc)) + fabsf (FMAT (h, n - 1, n - 2, nc));
      y = 0.75 * s;
      x = y;
      w = -0.4375 * s * s;
    }

  iterations++;

  for (m = n - 2; m >= e; m--)
    {
      double a1, a2, a3;

      z = FMAT (h, m, m, nc);
      r = x - z;
      s = y - z;
      p = FMAT (h, m, m + 1, nc) + (r * s - w) / FMAT (h, m + 1, m, nc);
      q = FMAT (h, m + 1, m + 1, nc) - z - r - s;
      r = FMAT (h, m + 2, m + 1, nc);
      s = fabsf (p) + fabsf (q) + fabsf (r);
      p /= s;
      q /= s;
      r /= s;

      if (m == e)
        break;

      a1 = fabsf (FMAT (h, m, m - 1, nc));
      a2 = fabsf (FMAT (h, m - 1, m - 1, nc));
      a3 = fabsf (FMAT (h, m + 1, m + 1, nc));

      if (a1 * (fabsf (q) + fabsf (r)) <= GSL_FLT_EPSILON * fabsf (p) * (a2 + a3))
        break;
    }

  for (i = m + 2; i <= n; i++)
    {
      FMAT (h, i, i - 2, nc) = 0;
    }

  for (i = m + 3; i <= n; i++)
    {
      FMAT (h, i, i - 3, nc) = 0;
    }

  /* double QR step */

  for (k = m; k <= n - 1; k++)
    {
      notlast = (k != n - 1);

      if (k != m)
        {
          p = FMAT (h, k, k - 1, nc);
          q = FMAT (h, k + 1, k - 1, nc);
          r = notlast ? FMAT (h, k + 2, k - 1, nc) : 0.0;

          x = fabsf (p) + fabsf (q) + fabsf (r);

          if (x == 0)
            continue;           /* FIXME????? */

          p /= x;
          q /= x;
          r /= x;
        }

      s = sqrtf (p * p + q * q + r * r);

      if (p < 0)
        s = -s;

      if (k != m)
        {
          FMAT (h, k, k - 1, nc) = -s * x;
        }
      else if (e != m)
        {
          FMAT (h, k, k - 1, nc) *= -1;
        }

      p += s;
      x = p / s;
      y = q / s;
      z = r / s;
      q /= p;
      r /= p;

      /* do row modifications */

      for (j = k; j <= n; j++)
        {
          p = FMAT (h, k, j, nc) + q * FMAT (h, k + 1, j, nc);

          if (notlast)
            {
              p += r * FMAT (h, k + 2, j, nc);
              FMAT (h, k + 2, j, nc) -= p * z;
            }

          FMAT (h, k + 1, j, nc) -= p * y;
          FMAT (h, k, j, nc) -= p * x;
        }

      j = (k + 3 < n) ? (k + 3) : n;

      /* do column modifications */

      for (i = e; i <= j; i++)
        {
          p = x * FMAT (h, i, k, nc) + y * FMAT (h, i, k + 1, nc);

          if (notlast)
            {
              p += z * FMAT (h, i, k + 2, nc);
              FMAT (h, i, k + 2, nc) -= p * r;
            }
          FMAT (h, i, k + 1, nc) -= p * q;
          FMAT (h, i, k, nc) -= p;
        }
    }

  goto next_iteration;
}

int
gsl_poly_complex_solve (const float * a, size_t n,
                        gsl_poly_complex_workspace_float * w,
                        gsl_complex_packed_float_ptr z)
{
	  int status;
	  float *m;
/*
	  if (n == 0)
	    {
	      GSL_ERROR ("number of terms must be a positive integer", GSL_EINVAL);
	    }

	  if (n == 1)
	    {
	      GSL_ERROR ("cannot solve for only one term", GSL_EINVAL);
	    }

	  if (a[n - 1] == 0)
	    {
	      GSL_ERROR ("leading term of polynomial must be non-zero", GSL_EINVAL) ;
	    }

	  if (w->nc != n - 1)
	    {
	      GSL_ERROR ("size of workspace does not match polynomial", GSL_EINVAL);
	    }
*/
	  m = w->matrix;

	  set_companion_matrix (a, n - 1, m);

	  balance_companion_matrix (m, n - 1);

	  status = qr_companion (m, n - 1, z);

//	  if (status)
//	    {
//	      GSL_ERROR("root solving qr method failed to converge", GSL_EFAILED);
//	    }

//	  return GSL_SUCCESS;
	  return status;
}
