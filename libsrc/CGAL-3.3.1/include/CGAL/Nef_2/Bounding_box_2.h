// Copyright (c) 1997-2000  Max-Planck-Institute Saarbruecken (Germany).
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.3-branch/Nef_2/include/CGAL/Nef_2/Bounding_box_2.h $
// $Id$
// 
//
// Author(s)     : Peter Hachenberger  <hachenberger@mpi-sb.mpg.de>

#ifndef CGAL_BOUNDING_BOX_D_H
#define CGAL_BOUNDING_BOX_D_H


#include<CGAL/Box_intersection_d/Box_d.h>

CGAL_BEGIN_NAMESPACE

// template<typename T, typename K> class Bounding_box_2;

template<class T,typename Kernel>
class Bounding_box_2 {

  typedef typename Kernel::Point_2 Point;
  typedef typename Kernel::Standard_point_2 SPoint;
  typedef typename Kernel::Standard_direction_2 SDirection;

 public:
  template<typename Vertex_iterator>
    Bounding_box_2(Vertex_iterator , Vertex_iterator ) {
    CGAL_assertion_msg(false, "dummy interface");
  }
  
  Point intersection_ray_bbox(const SPoint& , const SDirection& ) {
    CGAL_assertion_msg(false, "dummy interface");
    return Point();
  }
};


template<typename Kernel>
class Bounding_box_2<Tag_false,Kernel> 
  : Box_intersection_d::Box_d<typename Kernel::Standard_FT,2> {
  
  typedef typename Kernel::Standard_FT SFT;
  typedef typename Kernel::Standard_RT SRT;
  typedef typename Box_intersection_d::Box_d<SFT,2> Box;
  typedef typename Kernel::Point_2 Point;
  typedef typename Kernel::Standard_point_2 SPoint;
  typedef typename Kernel::Standard_direction_2 SDirection;
  //  typedef typename Kernel::Direction_2 Direction;
  typedef typename Kernel::Standard_line_2 SLine;

  template<typename Vertex_handle>
    static SFT* vertex2point(Vertex_handle v, SFT p[2]) {
    p[0] = v->point()[0];
    p[1] = v->point()[1];
    return p;
  }
  
 public:

#ifndef CGAL_CFG_USING_BASE_MEMBER_BUG_3
  using Box::min_coord;
  using Box::max_coord;
#endif

  template<typename Vertex_iterator>
    Bounding_box_2(Vertex_iterator begin, Vertex_iterator end) {
    SFT p[2];
    vertex2point(begin,p);
    *((Box*) this) = Box(p,p);
    //    (Box) *this = Box(p,p);
    for(++begin;begin != end; ++begin)
      extend(vertex2point(begin,p));
  }
  
  Point intersection_ray_bbox(const SPoint& p, const SDirection& d) {
    int dim = d.delta(0) == 0 ? 1 : 0;
    CGAL_assertion(d.delta(dim) != 0);
    SPoint minmax;
    if(dim == 0)
      minmax = d.delta(dim) < 0 
	? SPoint(this->min_coord(0).numerator(),SRT(0),this->min_coord(0).denominator()) 
	: SPoint(this->max_coord(0).numerator(),SRT(0),this->max_coord(0).denominator());
    else
      minmax = d.delta(dim) < 0 
	? SPoint(SRT(0),this->min_coord(0).numerator(),this->min_coord(0).denominator()) 
	: SPoint(SRT(0),this->max_coord(0).numerator(),this->max_coord(0).denominator());
    SLine l1(p,d);
    SLine l2 = dim == 0 
      ? SLine(minmax, SDirection(0,1)) 
      : SLine(minmax, SDirection(1,0));
    
    Object o = intersection(l1,l2);
    if(assign(minmax,o)) {
      Kernel K;
      return K.construct_point(minmax);
    }
    CGAL_assertion_msg(false, "code not robust - l2 must be constructed to"
		       " be non-collinear with l1");
    return Point();
  }
    
};

CGAL_END_NAMESPACE
#endif // CGAL_BOUNDING_BOX_D_H
