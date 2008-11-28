// Copyright (c) 1999  Utrecht University (The Netherlands),
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.3-branch/Kernel_23/include/CGAL/predicates/sign_of_determinant.h $
// $Id$
// 
//
// Author(s)     : Sylvain Pion
//                 Stefan Schirra

#ifndef CGAL_PREDICATES_SIGN_OF_DETERMINANT_H
#define CGAL_PREDICATES_SIGN_OF_DETERMINANT_H

#include <CGAL/determinant.h>

CGAL_BEGIN_NAMESPACE

template <class RT>
inline
typename Sgn<RT>::result_type
sign_of_determinant2x2( const RT& a00,  const RT& a01,
                        const RT& a10,  const RT& a11)
{
  return enum_cast<Sign>(CGAL_NTS compare( a00*a11, a10*a01));
}

template <class RT>
inline
typename Sgn<RT>::result_type
sign_of_determinant3x3( const RT& a00,  const RT& a01,  const RT& a02,
                        const RT& a10,  const RT& a11,  const RT& a12,
                        const RT& a20,  const RT& a21,  const RT& a22)
{
  return CGAL_NTS sign(det3x3_by_formula(a00, a01, a02,
                                         a10, a11, a12,
                                         a20, a21, a22));
}

template <class RT>
inline
typename Sgn<RT>::result_type
sign_of_determinant4x4(
 const RT& a00,  const RT& a01,  const RT& a02,  const RT& a03,
 const RT& a10,  const RT& a11,  const RT& a12,  const RT& a13,
 const RT& a20,  const RT& a21,  const RT& a22,  const RT& a23,
 const RT& a30,  const RT& a31,  const RT& a32,  const RT& a33)
{
  return CGAL_NTS sign(det4x4_by_formula(a00, a01, a02, a03,
                                         a10, a11, a12, a13,
                                         a20, a21, a22, a23,
                                         a30, a31, a32, a33));
}

template <class RT>
CGAL_KERNEL_LARGE_INLINE
typename Sgn<RT>::result_type
sign_of_determinant5x5(
 const RT& a00,  const RT& a01,  const RT& a02,  const RT& a03,  const RT& a04,
 const RT& a10,  const RT& a11,  const RT& a12,  const RT& a13,  const RT& a14,
 const RT& a20,  const RT& a21,  const RT& a22,  const RT& a23,  const RT& a24,
 const RT& a30,  const RT& a31,  const RT& a32,  const RT& a33,  const RT& a34,
 const RT& a40,  const RT& a41,  const RT& a42,  const RT& a43,  const RT& a44)
{
  return CGAL_NTS sign(det5x5_by_formula(a00, a01, a02, a03, a04,
                                         a10, a11, a12, a13, a14,
                                         a20, a21, a22, a23, a24,
                                         a30, a31, a32, a33, a34,
                                         a40, a41, a42, a43, a44));
}

template <class RT>
CGAL_KERNEL_LARGE_INLINE
typename Sgn<RT>::result_type
sign_of_determinant6x6(
 const RT& a00, const RT& a01, const RT& a02, const RT& a03, const RT& a04,
 const RT& a05,
 const RT& a10, const RT& a11, const RT& a12, const RT& a13, const RT& a14,
 const RT& a15,
 const RT& a20, const RT& a21, const RT& a22, const RT& a23, const RT& a24,
 const RT& a25,
 const RT& a30, const RT& a31, const RT& a32, const RT& a33, const RT& a34,
 const RT& a35,
 const RT& a40, const RT& a41, const RT& a42, const RT& a43, const RT& a44,
 const RT& a45,
 const RT& a50, const RT& a51, const RT& a52, const RT& a53, const RT& a54,
 const RT& a55)
{
  return CGAL_NTS sign(det6x6_by_formula(a00, a01, a02, a03, a04, a05,
                                         a10, a11, a12, a13, a14, a15,
                                         a20, a21, a22, a23, a24, a25,
                                         a30, a31, a32, a33, a34, a35,
                                         a40, a41, a42, a43, a44, a45,
                                         a50, a51, a52, a53, a54, a55));
}

CGAL_END_NAMESPACE

#endif // CGAL_PREDICATES_SIGN_OF_DETERMINANT_H
