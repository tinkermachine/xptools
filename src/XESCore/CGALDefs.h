/* 
 * Copyright (c) 2009, Laminar Research.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 *
 */

#ifndef CGALDefs_H
#define CGALDefs_H


#if USE_GMP
#include <CGAL/Gmpq.h>
#include <CGAL/Lazy_exact_nt.h>
#else
#include <CGAL/Quotient.h>
#include <CGAL/MP_Float.h>
#endif

#include <CGAL/Cartesian.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Filtered_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>

/******************************************************************************************************************************************************
 *
 ******************************************************************************************************************************************************/

#if USE_GMP
// Use GMP for our number type.  It appers to be maybe 10% faster than quotient<MP_float>.  We use the Lazy_exact_nt adapter to
// defer calculation where possible.  We must have an exact number type or inserting into maps (which is fundamental to ALL processing) can blow up.
typedef CGAL::Lazy_exact_nt<CGAL::Gmpq> NT;
#else
typedef CGAL::Lazy_exact_nt<CGAL::Quotient<CGAL::MP_Float> >  NT;
#endif

// Use the filtered kernel to answer predicates rapidly in easy cases.
// Lazy kernel seems to be slower, and optimized compile almost brings down a Mac Pro, which is NOT a good sign.
//typedef CGAL::Filtered_kernel<CGAL::Simple_cartesian<NT> > FastKernel;
typedef CGAL::Simple_cartesian<NT>  FastKernel;


// This is very, very dangerous.  Basically this creates the illusion of a well-defined "sqrt" function for our numeric type.  Why is that dangerous?
// Well, our number type advertises as exact constructions, meaning the math comes out perfect.  But this sqrt is defined via cast to double, so it
// is very imperfect.  So...some CGAL algs that require sqrt might blow up.
//
// Why did I do it?  The delauney mesh conformer requires it, and doesn't terribly need a good sqrt - it has to "pick" a "decent" split point - being
// off a little won't help, and the split makes such a huge mesh change that the operation isn't re-evaluted.

CGAL_BEGIN_NAMESPACE
CGAL_NTS_BEGIN_NAMESPACE
inline FastKernel::FT sqrt(FastKernel::FT n)
{
	return ::sqrtf(CGAL::to_double(n));
}
CGAL_NTS_END_NAMESPACE
CGAL_END_NAMESPACE


typedef CGAL::Bbox_2									Bbox_2;
typedef FastKernel::Point_2                             Point_2;
typedef FastKernel::Vector_2                            Vector_2;
typedef FastKernel::Triangle_2							Triangle_2;
typedef FastKernel::Point_3                             Point_3;
typedef FastKernel::Vector_3                            Vector_3;
typedef FastKernel::Plane_3                             Plane_3;
typedef FastKernel::Segment_2                           Segment_2;
typedef CGAL::Line_2<FastKernel>                        Line_2;
typedef FastKernel::Ray_2								Ray_2;
typedef CGAL::Polygon_2<FastKernel>						Polygon_2;					// Ben says: this only works because GPS polygon uses "standard" kernel polygons.  If this was not
typedef CGAL::Polygon_with_holes_2<FastKernel>			Polygon_with_holes_2;		// true, we could use these definitons from our GPS segment traits.  This allows us to have polygons
																					// Without GPS polygons!


inline CGAL::Bbox_2& operator+=(Bbox_2& lhs, const Bbox_2& rhs)
{
	lhs = lhs + rhs;
	return lhs;
}

#endif /* CGALDefs_H */
