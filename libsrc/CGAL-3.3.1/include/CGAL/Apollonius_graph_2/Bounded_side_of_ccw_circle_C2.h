// Copyright (c) 2003,2004,2006  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.3-branch/Apollonius_graph_2/include/CGAL/Apollonius_graph_2/Bounded_side_of_ccw_circle_C2.h $
// $Id$
// 
//
// Author(s)     : Menelaos Karavelas <mkaravel@cse.nd.edu>



#ifndef CGAL_APOLLONIUS_GRAPH_2_BOUNDED_SIDE_OF_CCW_CIRCLE_C2_H
#define CGAL_APOLLONIUS_GRAPH_2_BOUNDED_SIDE_OF_CCW_CIRCLE_C2_H

#include <CGAL/Apollonius_graph_2/basic.h>
#include <CGAL/Apollonius_graph_2/Predicate_constructions_C2.h>


CGAL_BEGIN_NAMESPACE

CGAL_APOLLONIUS_GRAPH_2_BEGIN_NAMESPACE

//--------------------------------------------------------------------

template< class K >
class Sign_of_Voronoi_radius
{
public:
  typedef Voronoi_radius_2<K>   Voronoi_radius;
  typedef typename K::FT        FT;
  typedef typename K::Sign      Sign;

public:

  inline
  Sign operator()(const Voronoi_radius& vr, const Field_with_sqrt_tag&) const
    {
      FT r = vr.c1() + vr.c2() * CGAL::sqrt(vr.delta());
      return CGAL::sign(r);
    }


  inline
  Sign operator()(const Voronoi_radius& vr, const Integral_domain_without_division_tag&) const
    {
      // this is another way of doing this; the degree becomes 10
      // instead of 5 in this case.
      //      return sign_a_plus_b_x_sqrt_c(vr.c1(), vr.c2(), vr.delta());

      bool is_first_root = CGAL::is_negative(vr.c2());

      Sign s_beta = CGAL::sign(vr.b());
      Sign s_gamma = CGAL::sign(vr.c());

      // the existence test
      Sign sign_of_root;
      if ( is_first_root ) {
	sign_of_root = sign_of_first_root(s_beta, s_gamma);
      } else {
	sign_of_root = sign_of_second_root(s_beta, s_gamma);
      }
      return sign_of_root;
    }
};

template< class K >
class Bounded_side_of_CCW_circle_2
{
private:
  Sign_of_Voronoi_radius<K> test;

public:
  typedef Voronoi_radius_2<K>       Voronoi_radius;
  typedef typename K::Bounded_side  Bounded_dide;

public:
  template<class Tag>
  inline Bounded_side
  operator()(const Voronoi_radius& vr, const Tag& tag)
    {
      return enum_cast<Bounded_side>(  -test(vr, tag )  );
    }
};


//--------------------------------------------------------------------

CGAL_APOLLONIUS_GRAPH_2_END_NAMESPACE

CGAL_END_NAMESPACE

#endif // CGAL_APOLLONIUS_GRAPH_2_BOUNDED_SIDE_OF_CCW_CIRCLE_C2_H
