// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/pointing_segment.hpp>

#include "math/vector.hpp"
/*
#include <cstddef>

#include <boost/type_traits/is_arithmetic.hpp>

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/tags.hpp>

namespace boost { namespace geometry {

namespace traits {
namespace detail {

template <bool>
struct vector_tag
{
    typedef geometry_not_recognized_tag type;
};


template <>
struct vector_tag<true>
{
    typedef point_tag type;
};

} // namespace detail

// Assign the point-tag, preventing arrays of points getting a point-tag
template <>//typename CoordinateType = float, std::size_t DimensionCount>
struct tag<rt::vector_t>
    : detail::vector_tag<boost::is_arithmetic<float>::value> {};


template <>//typename CoordinateType = float, std::size_t DimensionCount>
struct coordinate_type<rt::vector_t>
{
    typedef float type;
};

template <>//typename CoordinateType = float, std::size_t DimensionCount>
struct dimension<rt::vector_t>: boost::mpl::int_<3> {};

template <std::size_t Dimension>
struct access<rt::vector_t, Dimension>
{
    static inline float get(rt::vector_t p)
    {
        return p[Dimension];
    }

    static inline void set(rt::vector_t p, float const value)
    {
        p[Dimension] = value;
    }
};


} // namespace traits

}} // namespace boost::geometry


#define BOOST_GEOMETRY_REGISTER_VECTOR_CS(CoordinateSystem) \
    namespace boost { namespace geometry { namespace traits { \
    template <> \
    struct coordinate_system<rt::vector_t> \
    { \
        typedef CoordinateSystem type; \
    }; \
    }}}

BOOST_GEOMETRY_REGISTER_VECTOR_CS(cs::cartesian)

//namespace rt {
//
//typedef geo::model::box<vector_t> box_t;
//typedef geo::model::segment<vector_t> segment_t;
//
//}
*/
namespace geo = boost::geometry;

namespace rt {

typedef geo::model::point<float, 3, geo::cs::cartesian> point_t;

inline const point_t&
to_point(const vector_t& vector)
{
	return reinterpret_cast<const point_t&>(vector);
}

inline point_t&
to_point(vector_t& vector)
{
	return reinterpret_cast<point_t&>(vector);
}

}
