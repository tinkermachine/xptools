// Copyright (c) 2000  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.3-branch/Cartesian_kernel/include/CGAL/Cartesian/Vector_2.h $
// $Id$
// 
//
// Author(s)     : Andreas Fabri, Herve Bronnimann

#ifndef CGAL_CARTESIAN_VECTOR_2_H
#define CGAL_CARTESIAN_VECTOR_2_H

#include <CGAL/Origin.h>
#include <CGAL/Twotuple.h>
#include <CGAL/constant.h>

CGAL_BEGIN_NAMESPACE

template < class R_ >
class VectorC2
{
  typedef typename R_::FT                   FT;
  typedef typename R_::Point_2              Point_2;
  typedef typename R_::Vector_2             Vector_2;
  typedef typename R_::Segment_2            Segment_2;
  typedef typename R_::Ray_2                Ray_2;
  typedef typename R_::Line_2               Line_2;
  typedef typename R_::Direction_2          Direction_2;

  typedef Twotuple<FT>	                           Rep;
  typedef typename R_::template Handle<Rep>::type  Base;

  Base base;

public:
  typedef R_                                     R;

  VectorC2() {}
 
  VectorC2(const FT &x, const FT &y)
    : base(x, y) {}

  VectorC2(const FT &hx, const FT &hy, const FT &hw)
  {
    if (hw != FT(1))
      base = Rep(hx/hw, hy/hw);
    else
      base = Rep(hx, hy);
  }

  const FT & x() const
  {
      return get(base).e0;
  }
  const FT & y() const
  {
      return get(base).e1;
  }

  const FT & hx() const
  {
      return x();
  }
  const FT & hy() const
  {
      return y();
  }
  const FT& hw() const
  {
    return constant<FT, 1>();
  }

};

template < class R >
CGAL_KERNEL_INLINE
bool 
operator==(const VectorC2<R> &v, const VectorC2<R> &w)
{
  return w.x() == v.x() && w.y() == v.y();
}

template < class R >
inline
bool 
operator!=(const VectorC2<R> &v, const VectorC2<R> &w)
{
  return !(v == w);
}

template < class R >
inline
bool
operator==(const VectorC2<R> &v, const Null_vector &)
{
  return CGAL_NTS is_zero(v.x()) && CGAL_NTS is_zero(v.y());
}

template < class R >
inline
bool
operator==(const Null_vector &n, const VectorC2<R> &v)
{
  return v == n;
}

template < class R >
inline
bool
operator!=(const VectorC2<R> &v, const Null_vector &n)
{
  return !(v == n);
}

template < class R >
inline
bool
operator!=(const Null_vector &n, const VectorC2<R> &v)
{
  return !(v == n);
}

CGAL_END_NAMESPACE

#endif // CGAL_CARTESIAN_VECTOR_2_H
