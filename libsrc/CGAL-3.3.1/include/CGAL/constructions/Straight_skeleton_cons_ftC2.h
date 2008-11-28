// Copyright (c) 2006 Fernando Luis Cacciola Carballal. All rights reserved.
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.3-branch/Straight_skeleton_2/include/CGAL/constructions/Straight_skeleton_cons_ftC2.h $
// $Id$
//
// Author(s)     : Fernando Cacciola <fernando_cacciola@ciudad.com.ar>
//
#ifndef CGAL_STRAIGHT_SKELETON_CONS_FTC2_H
#define CGAL_STRAIGHT_SKELETON_CONS_FTC2_H 1

#include <CGAL/CORE_BigFloat.h>

CGAL_BEGIN_NAMESPACE 

namespace CGAL_SS_i
{

template<class NT>
inline NT inexact_sqrt( NT const& n )
{
  return CGAL_NTS sqrt(n);
}

inline MP_Float inexact_sqrt( MP_Float const& n )
{
  CORE::BigFloat nn( CGAL::to_double(n) ) ;
  CORE::BigFloat s = CORE::sqrt(nn);
  return MP_Float(s.doubleValue());
}

inline Quotient<MP_Float> inexact_sqrt( Quotient<MP_Float> const& q )
{
  CGAL_precondition(q > 0);
  return Quotient<MP_Float>(CGAL_SS_i::inexact_sqrt(q.numerator()*q.denominator())
                           ,q.denominator()
                           );
}


// Given an oriented 2D straight line segment 'e', computes the normalized coefficients (a,b,c) of the
// supporting line.
// POSTCONDITION: [a,b] is the leftward normal _unit_ (a�+b�=1) vector.
// POSTCONDITION: In case of overflow, an empty optional<> is returned.
template<class K>
optional< Line_2<K> > compute_normalized_line_ceoffC2( Segment_2<K> const& e )
{
  bool finite = true ;
  
  typedef typename K::FT FT ;
  
  FT a (0.0),b (0.0) ,c(0.0)  ;

  if(e.source().y() == e.target().y())
  {
    a = 0 ;
    if(e.target().x() > e.source().x())
    {
      b = 1;
      c = -e.source().y();
    }
    else if(e.target().x() == e.source().x())
    {
      b = 0;
      c = 0;
    }
    else
    {
      b = -1;
      c = e.source().y();
    }

    CGAL_STSKEL_TRAITS_TRACE("Line coefficients for HORIZONTAL line:\n" 
                            << s2str(e) << " a="<< n2str(a) << ", b=" << n2str(b) << ", c=" << n2str(c)
                           ) ;
  }
  else if(e.target().x() == e.source().x())
  {
    b = 0;
    if(e.target().y() > e.source().y())
    {
      a = -1;
      c = e.source().x();
    }
    else if (e.target().y() == e.source().y())
    {
      a = 0;
      c = 0;
    }
    else
    {
      a = 1;
      c = -e.source().x();
    }

    CGAL_STSKEL_TRAITS_TRACE("Line coefficients for VERTICAL line:\n"
                            << s2str(e) << " a="<< n2str(a) << ", b=" << n2str(b) << ", c=" << n2str(c)
                           ) ;
  }
  else
  {
    FT sa = e.source().y() - e.target().y();
    FT sb = e.target().x() - e.source().x();
    FT l2 = (sa*sa) + (sb*sb) ;

    if ( CGAL_NTS is_finite(l2) )
    {
      FT l = CGAL_SS_i :: inexact_sqrt(l2);
  
      a = sa / l ;
      b = sb / l ;
  
      c = -e.source().x()*a - e.source().y()*b;
    }
    else finite = false ;
    
    CGAL_STSKEL_TRAITS_TRACE("Line coefficients for line:\n"
                            << s2str(e) << " a="<< n2str(a) << ", b=" << n2str(b) << ", c=" << n2str(c)
                           ) ;
  }
  
  if ( finite )
    if ( !CGAL_NTS is_finite(a) || !CGAL_NTS is_finite(b) || !CGAL_NTS is_finite(c) ) 
      finite = false ;

  return cgal_make_optional( finite, K().construct_line_2_object()(a,b,c) ) ;
}

template<class FT>
FT squared_distance_from_point_to_lineC2( FT const& px, FT const& py, FT const& sx, FT const& sy, FT const& tx, FT const& ty )
{
  FT ldx = tx - sx ;
  FT ldy = ty - sy ;
  FT rdx = sx - px ;
  FT rdy = sy - py ;
  
  FT n = CGAL_NTS square(ldx * rdy - rdx * ldy);
  FT d = CGAL_NTS square(ldx) + CGAL_NTS square(ldy);
  
  return n / d ;
}

//
// Constructs a Trisegment_2 which stores 3 oriented straight line segments e0,e1,e2 along with their collinearity.
//
// NOTE: If the collinearity cannot be determined reliably, a null trisegment is returned.
//
template<class K>
Trisegment_2<K> construct_trisegment ( Segment_2<K> const& e0
                                     , Segment_2<K> const& e1
                                     , Segment_2<K> const& e2
                                     )
{
  typedef Trisegment_2<K> Trisegment_2 ;
  Uncertain<Trisegment_collinearity> lCollinearity = certified_trisegment_collinearity(e0,e1,e2);
  
  if ( !is_indeterminate(lCollinearity) ) 
       return Trisegment_2(e0, e1, e2, lCollinearity) ;
  else return Trisegment_2::null() ;
}

// Constructs a seeded trisegment which stores the main trisegment and the trisegments for its seed nodes.
//
// NOTE: Any of the seed trisegments can be null, which means that the current event does not emerge from a previous event but
// directly from the contour (it is an initial event rather than propagation event)
//
template<class K>
Seeded_trisegment_2<K> construct_seeded_trisegment ( Trisegment_2<K> const& event 
                                                   , Trisegment_2<K> const& lseed
                                                   , Trisegment_2<K> const& rseed
                                                   )
{
  return Seeded_trisegment_2<K>(event,lseed,rseed);
}


// Constructs a seeded trisegment that corresponds to an initial event (that is with no seeds)
//
template<class K>
Seeded_trisegment_2<K> construct_seeded_trisegment ( Trisegment_2<K> const& event )
{
  return Seeded_trisegment_2<K>(event);
}


// Given 3 oriented straight line segments: e0, e1, e2 
// returns the OFFSET DISTANCE (n/d) at which the offsetted lines
// intersect at a single point, IFF such intersection exist.
// If the lines intersect to the left, the returned distance is positive.
// If the lines intersect to the right, the returned distance is negative.
// If the lines do not intersect, for example, for collinear edges, or parallel edges but with the same orientation,
// returns 0 (the actual distance is undefined in this case, but 0 is a usefull return)
//
// NOTE: The result is a explicit rational number returned as a tuple (num,den); the caller must check that den!=0 manually
// (a predicate for instance should return indeterminate in this case)
//
// PRECONDITION: None of e0, e1 and e2 are collinear (but two of them can be parallel)
//
// POSTCONDITION: In case of overflow an empty optional is returned.
//
// NOTE: The segments (e0,e1,e2) are stored in the argument as the trisegment st.event()
//
template<class K>
optional< Rational< typename K::FT> > compute_normal_offset_lines_isec_timeC2 ( Seeded_trisegment_2<K> const& st )
{
  typedef typename K::FT  FT ;
  
  typedef Line_2<K> Line_2 ;
  
  typedef optional<Line_2> Optional_line_2 ;
  
  CGAL_STSKEL_TRAITS_TRACE("Computing normal offset lines isec time for: " << st ) ;
  
  FT num(0.0), den(0.0) ;
  
  // DETAILS:
  //
  // An offset line is given by:
  //
  //   a*x(t) + b*y(t) + c - t = 0
  //
  // were 't > 0' being to the left of the line.
  // If 3 such offset lines intersect at the same offset distance, the intersection 't',
  // or 'time', can be computed solving for 't' in the linear system formed by 3 such equations.
  // The result is :
  //
  //  t = a2*b0*c1 - a2*b1*c0 - b2*a0*c1 + b2*a1*c0 + b1*a0*c2 - b0*a1*c2
  //      ---------------------------------------------------------------
  //             -a2*b1 + a2*b0 + b2*a1 - b2*a0 + b1*a0 - b0*a1 ;

  bool ok = false ;
  
  Optional_line_2 l0 = compute_normalized_line_ceoffC2(st.event().e0()) ;
  Optional_line_2 l1 = compute_normalized_line_ceoffC2(st.event().e1()) ;
  Optional_line_2 l2 = compute_normalized_line_ceoffC2(st.event().e2()) ;

  if ( l0 && l1 && l2 )
  {
    num = (l2->a()*l0->b()*l1->c())
         -(l2->a()*l1->b()*l0->c())
         -(l2->b()*l0->a()*l1->c())
         +(l2->b()*l1->a()*l0->c())
         +(l1->b()*l0->a()*l2->c())
         -(l0->b()*l1->a()*l2->c());
      
    den = (-l2->a()*l1->b())
         +( l2->a()*l0->b())
         +( l2->b()*l1->a())
         -( l2->b()*l0->a())
         +( l1->b()*l0->a())
         -( l0->b()*l1->a());
         
    ok = CGAL_NTS is_finite(num) && CGAL_NTS is_finite(den);     
  }
  
  CGAL_STSKEL_TRAITS_TRACE("Event time (normal): n=" << num << " d=" << den << " n/d=" << Rational<FT>(num,den)  )

  return cgal_make_optional(ok,Rational<FT>(num,den)) ;
}

// Given two oriented straight line segments e0 and e1 such that e-next follows e-prev, returns
// the coordinates of the midpoint of the segment between e-prev and e-next.
// NOTE: the edges can be oriented e0->e1 or e1->e0
//
// POSTCONDITION: In case of overflow an empty optional is returned.
//
template<class K>
optional< Point_2<K> > compute_oriented_midpoint ( Segment_2<K> const& e0, Segment_2<K> const& e1 )
{
  bool ok = false ;
  
  typedef typename K::FT FT ;
  
  FT delta01 = CGAL::squared_distance(e0.target(),e1.source());
  FT delta10 = CGAL::squared_distance(e1.target(),e0.source());
  
  Point_2<K> mp ;
   
  if ( CGAL_NTS is_finite(delta01) &&  CGAL_NTS is_finite(delta10) )
  {
    if ( delta01 <= delta10 )
         mp = CGAL::midpoint(e0.target(),e1.source());
    else mp = CGAL::midpoint(e1.target(),e0.source());
    
    CGAL_STSKEL_TRAITS_TRACE("Computing oriented midpoint between:"
                             << "\ne0: " << s2str(e0)
                             << "\ne1: " << s2str(e1)
                             << "\nmp=" << p2str(mp)
                            );
                            
    ok = CGAL_NTS is_finite(mp.x()) && CGAL_NTS is_finite(mp.y());
  }
  
  return cgal_make_optional(ok,mp);
}


//
// Given 3 oriented straight line segments: e0, e1, e2 and the corresponding offseted segments: e0*, e1* and e2*,
// returns the point of the left or right seed (offset vertex) (e0*,e1*) or (e1*,e2*) 
// 
// If the current event (defined by e0,e1,e2) is a propagated event, that is, it follows from a previous event,
// the seeds are skeleten nodes and are given by non-null trisegments.
// If the current event is an initial event the seeds are contour vertices and are given by null trisegmets.
//
// If a seed is a skeleton node, its point has to be computed from the trisegment that defines it.
// That trisegment is exactly the trisegment that defined then previous event which produced the skeleton node
// (so the trisegment is basically a lazy representation of the seed point).
//
// If a seed is a contour vertex, its point is then simply the target endoint of e0 or e1 (for the left/right seed).
//
// This method returns the specified seed point (left or right)
//
// NOTE: Split events involve 3 edges but only one seed, the left (that is, only e0*,e1* is connected before the event).
// The seeded trisegment for a split event has always a null right seed even if the event is not an initial event
// (in which case its left seed won't be null).
// If you ask for the right seed point for a seeded trisegment corresponding to a split event you will just get e1.target()
// which is nonsensical for a non initial split event.
//
// NOTE: There is an abnormal collinearity case which ocurrs when e0 and e2 are collinear.
// In this case, these lines do not correspond to an offset vertex (because e0* and e2* are never consecutive before the event),
// so the degenerate seed is neither the left or the right seed. In this case, the SEED ID for the degenerate pseudo seed is UNKOWN.
// If you request the point of such degenerate pseudo seed the oriented midpoint bettwen e0 and e2 is returned.
//
template<class K>
optional< Point_2<K> > compute_seed_pointC2 ( Seeded_trisegment_2<K> const& st, typename Trisegment_2<K>::SEED_ID sid )
{
  optional< Point_2<K> > p ;

  typedef Trisegment_2<K> Trisegment_2 ;
    
  switch ( sid )
  {
    case Trisegment_2::LEFT :
         
       p = st.lseed().is_null() ? compute_oriented_midpoint(st.event().e0(),st.event().e1())
                                : construct_offset_lines_isecC2(construct_seeded_trisegment(st.lseed())) ;
       break ;                     
             
    case Trisegment_2::RIGHT : 
    
      p = st.rseed().is_null() ? compute_oriented_midpoint(st.event().e1(),st.event().e2())
                               : construct_offset_lines_isecC2(construct_seeded_trisegment(st.rseed())) ;
      break ;        
             
    case Trisegment_2::UNKNOWN : 
    
      p = compute_oriented_midpoint(st.event().e0(),st.event().e2());
      
      break ;
  }
  
  return p ;  
}

//
// Given the seeded trisegment for an event which is known to have a normal collinearity returns the seed point
// of the degenerate seed.
// A normal collinearity occurs when e0,e1 or e1,e2 are collinear.
template<class K>
optional< Point_2<K> > compute_degenerate_seed_pointC2 ( Seeded_trisegment_2<K> const& st )
{
  return compute_seed_pointC2( st, st.event().degenerate_seed_id() ) ;
}

// Given 3 oriented straight line segments: e0, e1, e2 
// such that two and only two of these edges are collinear, not neccesarily consecutive but with the same orientaton;
// returns the OFFSET DISTANCE (n/d) at which a line perpendicular to the collinear edge passing through
// the degenerate seed point intersects the offset line of the non collinear edge
//
// NOTE: The result is a explicit rational number returned as a tuple (num,den); the caller must check that den!=0 manually
// (a predicate for instance should return indeterminate in this case)
//
// POSTCONDITION: In case of overflow an empty optional is returned.
//
template<class K>
optional< Rational< typename K::FT> > compute_degenerate_offset_lines_isec_timeC2 ( Seeded_trisegment_2<K> const& st )
{
  typedef typename K::FT FT ;
  
  typedef Point_2<K> Point_2 ;
  typedef Line_2 <K> Line_2 ;
  
  typedef optional<Point_2> Optional_point_2 ;
  typedef optional<Line_2>  Optional_line_2 ;
  
  CGAL_STSKEL_TRAITS_TRACE("Computing degenerate offset lines isec time for: " << st ) ;
  
  // DETAILS:
  //
  // For simplicity, assume e0,e1 are the collinear edges.
  //
  //   (1)
  //   The bisecting line of e0 and e1 is a line perpendicular to e0 (and e1)
  //   which passes through 'q': the degenerate offset vertex (e0*,e1*)
  //   This "degenerate" bisecting line is given by:
  //
  //     B0(t) = p + t*[l0.a,l0.b]
  //
  //   where p is the projection of q along l0 and l0.a,l0.b are the _normalized_ line coefficients for e0 (or e1 which is the same)
  //   Since [a,b] is a _unit_ vector pointing perpendicularly to the left of e0 (and e1);
  //   any point B0(k) is at a distance k from the line supporting e0 and e1.
  //
  //   (2)
  //   The bisecting line of e0 and e2 is given by the following SEL
  //
  //    l0.a*x(t) + l0.b*y(t) + l0.c + t = 0
  //    l2.a*x(t) + l2.b*y(t) + l2.c + t = 0
  //
  //   where (l0.a,l0.b,l0.c) and (l2.a,l2.b,l0.c) are the normalized line coefficientes of e0 and e2 resp.
  //
  //     B1(t)=[x(t),y(t)]
  //
  //   (3)
  //   These two bisecting lines B0(t) and B1(t) intersect (if they do) in a single point 'p' whose distance
  //   to the lines supporting the 3 edges is exactly 't' (since those expressions are precisely parametrized in a distance)
  //   Solving the following vectorial equation:
  //
  //     [x(y),y(t)] = q + t*[l0.a,l0.b]
  //
  //   for t gives the result we want.
  //
  //
  bool ok = false ;

  Optional_line_2 l0 = compute_normalized_line_ceoffC2(st.event().collinear_edge    ()) ;
  Optional_line_2 l2 = compute_normalized_line_ceoffC2(st.event().non_collinear_edge()) ;

  Optional_point_2 q = compute_degenerate_seed_pointC2(st);
  
  FT num(0.0), den(0.0) ;

  if ( l0 && l2 && q )
  {
    FT px, py ;
    line_project_pointC2(l0->a(),l0->b(),l0->c(),q->x(),q->y(),px,py); 
    
    CGAL_STSKEL_TRAITS_TRACE("Seed point: " << p2str(*q) << ".\nProjected seed point: (" << n2str(px) << "," << n2str(py) << ")" ) ;
    
    if ( ! CGAL_NTS is_zero(l0->b()) ) // Non-vertical
    {
      num = (l2->a() * l0->b() - l0->a() * l2->b() ) * px + l0->b() * l2->c() - l2->b() * l0->c() ;
      den = (l0->a() * l0->a() - 1) * l2->b() + ( 1 - l2->a() * l0->a() ) * l0->b() ;
      
      CGAL_STSKEL_TRAITS_TRACE("Event time (degenerate, non-vertical) n=" << n2str(num) << " d=" << n2str(den) << " n/d=" << Rational<FT>(num,den) )
    }
    else
    {
      num = (l2->a() * l0->b() - l0->a() * l2->b() ) * py - l0->a() * l2->c() + l2->a() * l0->c() ;
      den = l0->a() * l0->b() * l2->b() - l0->b() * l0->b() * l2->a() + l2->a() - l0->a() ;
      
      CGAL_STSKEL_TRAITS_TRACE("Event time (degenerate, vertical) n=" << n2str(num) << " d=" << n2str(den) << " n/d=" << Rational<FT>(num,den) )
    }
    
    ok = CGAL_NTS is_finite(num) && CGAL_NTS is_finite(den);     
  }
  

  return cgal_make_optional(ok,Rational<FT>(num,den)) ;
}

//
// Calls the appropiate function depending on the collinearity of the edges.
//
template<class K>
optional< Rational< typename K::FT > > compute_offset_lines_isec_timeC2 ( Seeded_trisegment_2<K> const& st )
{
  CGAL_precondition ( st.event().collinearity() != TRISEGMENT_COLLINEARITY_ALL ) ;
 
  return st.event().collinearity() == TRISEGMENT_COLLINEARITY_NONE ? compute_normal_offset_lines_isec_timeC2    (st)
                                                                   : compute_degenerate_offset_lines_isec_timeC2(st);
}


// Given 3 oriented line segments e0, e1 and e2 
// such that their offsets at a certian distance intersect in a single point, 
// returns the coordinates (x,y) of such a point.
//
// PRECONDITIONS:
// None of e0, e1 and e2 are collinear (but two of them can be parallel)
// The line coefficients must be normalized: a�+b�==1 and (a,b) being the leftward normal vector
// The offsets at a certain distance do intersect in a single point.
//
// POSTCONDITION: In case of overflow an empty optional is returned.
//
template<class K>
optional< Point_2<K> > construct_normal_offset_lines_isecC2 ( Seeded_trisegment_2<K> const& st )
{
  typedef typename K::FT  FT ;
  
  typedef Point_2<K> Point_2 ;
  typedef Line_2<K>  Line_2 ;
  
  typedef optional<Line_2>  Optional_line_2 ;
  
  CGAL_STSKEL_TRAITS_TRACE("Computing normal offset lines isec point for: " << st ) ;
  
  FT x(0.0),y(0.0) ;
  
  Optional_line_2 l0 = compute_normalized_line_ceoffC2(st.event().e0()) ;
  Optional_line_2 l1 = compute_normalized_line_ceoffC2(st.event().e1()) ;
  Optional_line_2 l2 = compute_normalized_line_ceoffC2(st.event().e2()) ;

  bool ok = false ;
  
  if ( l0 && l1 && l2 )
  {
    FT den = l0->a()*l2->b() - l0->a()*l1->b() - l1->a()*l2->b() + l2->a()*l1->b() + l0->b()*l1->a() - l0->b()*l2->a();
  
    CGAL_STSKEL_TRAITS_TRACE("Event Point: d=" << n2str(den) )
  
    if ( ! CGAL_NTS certified_is_zero(den) ) 
    {
      FT numX = l0->b()*l2->c() - l0->b()*l1->c() - l1->b()*l2->c() + l2->b()*l1->c() + l1->b()*l0->c() - l2->b()*l0->c();
      FT numY = l0->a()*l2->c() - l0->a()*l1->c() - l1->a()*l2->c() + l2->a()*l1->c() + l1->a()*l0->c() - l2->a()*l0->c();
    
      if ( CGAL_NTS is_finite(den) && CGAL_NTS is_finite(numX) && CGAL_NTS is_finite(numY)  )
      {
        ok = true ;
        
        x =  numX / den ;
        y = -numY / den ;
        
      }
    }
  }
    
  CGAL_STSKEL_TRAITS_TRACE("\nNormal event point: x=" << n2str(x) << " y=" << n2str(y) )
    
  return cgal_make_optional(ok,K().construct_point_2_object()(x,y)) ;
}

// Given 3 oriented line segments e0, e1 and e2
// such that their offsets at a certian distance intersect in a single point, 
// returns the coordinates (x,y) of such a point.
// two and only two of the edges are collinear, not neccesarily consecutive but with the same orientaton
//
// PRECONDITIONS:
// The line coefficients must be normalized: a�+b�==1 and (a,b) being the leftward normal vector
// The offsets at a certain distance do intersect in a single point.
//
// POSTCONDITION: In case of overflow an empty optional is returned.
//
template<class K>
optional< Point_2<K> > construct_degenerate_offset_lines_isecC2 ( Seeded_trisegment_2<K> const& st )
{
  typedef typename K::FT FT ;
  
  typedef Point_2<K> Point_2 ;
  typedef Line_2<K>  Line_2 ;
  
  typedef optional<Point_2> Optional_point_2 ;
  typedef optional<Line_2>  Optional_line_2 ;
  
  CGAL_STSKEL_TRAITS_TRACE("Computing degenerate offset lines isec point for: " << st )  ;
  
  FT x(0.0),y(0.0) ;

  Optional_line_2 l0 = compute_normalized_line_ceoffC2(st.event().collinear_edge    ()) ;
  Optional_line_2 l2 = compute_normalized_line_ceoffC2(st.event().non_collinear_edge()) ;
  
  Optional_point_2 q = compute_degenerate_seed_pointC2(st);

  bool ok = false ;
  
  if ( l0 && l2 && q )
  {
    FT num, den ;
    
    FT px, py ;
    line_project_pointC2(l0->a(),l0->b(),l0->c(),q->x(),q->y(),px,py); 
    
    CGAL_STSKEL_TRAITS_TRACE("Seed point: " << p2str(*q) << ". Projected seed point: (" << n2str(px) << "," << n2str(py) << ")" ) ;
    
    if ( ! CGAL_NTS is_zero(l0->b()) ) // Non-vertical
    {
      num = (l2->a() * l0->b() - l0->a() * l2->b() ) * px + l0->b() * l2->c() - l2->b() * l0->c() ;
      den = (l0->a() * l0->a() - 1) * l2->b() + ( 1 - l2->a() * l0->a() ) * l0->b() ;
    }
    else
    {
      num = (l2->a() * l0->b() - l0->a() * l2->b() ) * py - l0->a() * l2->c() + l2->a() * l0->c() ;
      den = l0->a() * l0->b() * l2->b() - l0->b() * l0->b() * l2->a() + l2->a() - l0->a() ;
    }
  
    if ( ! CGAL_NTS certified_is_zero(den) && CGAL_NTS is_finite(den) && CGAL_NTS is_finite(num) )
    {
      x = px + l0->a() * num / den  ;
      y = py + l0->b() * num / den  ;
      
      ok = CGAL_NTS is_finite(x) && CGAL_NTS is_finite(y) ;
    }
  }
  

  CGAL_STSKEL_TRAITS_TRACE("\nDegenerate " << (CGAL_NTS is_zero(l0->b()) ? "(vertical)" : "") << " event point:  x=" << n2str(x) << " y=" << n2str(y) )

  return cgal_make_optional(ok,K().construct_point_2_object()(x,y)) ;
}

//
// Calls the appropiate function depending on the collinearity of the edges.
//
template<class K>
optional< Point_2<K> > construct_offset_lines_isecC2 ( Seeded_trisegment_2<K> const& st )
{
  CGAL_precondition ( st.event().collinearity() != TRISEGMENT_COLLINEARITY_ALL ) ;
  
  return st.event().collinearity() == TRISEGMENT_COLLINEARITY_NONE ? construct_normal_offset_lines_isecC2    (st)
                                                                   : construct_degenerate_offset_lines_isecC2(st) ;
}

// Give a point (px,py) and 3 oriented straight line segments e0,e1 and e2.
// such that their offsets at a certian distance intersect in a single point (ix,iy),
// returns the squared distance between (px,py) and (ix,iy)
//
// NOTE: e0,e1 _can_ be collinear, but e2 must not be collinear with e0 nor e1.
//
// PRECONDITIONS:
// The line coefficients must be normalized: a�+b�==1 and (a,b) being the leftward normal vector
// The offsets at a certain distance do intersect in a single point.
//
template<class K>
optional< typename K::FT> compute_offset_lines_isec_dist_to_pointC2 ( optional< Point_2<K> > const& p
                                                                    , Seeded_trisegment_2<K> const& st
                                                                    )
{
  typedef typename K::FT FT ;
  
  typedef Point_2<K> Point_2 ;
  
  typedef optional<Point_2>  Optional_point_2 ;
  
  bool ok = false ;
  
  FT sdist(0.0) ;
  
  if ( p )
  {
    Optional_point_2 i = construct_offset_lines_isecC2(st);
    
    if ( i )
    {
      FT dx  = i->x() - p->x() ;
      FT dy  = i->y() - p->y() ;
      FT dx2 = dx * dx ;
      FT dy2 = dy * dy ;
    
      sdist = dx2 + dy2 ;
      
      ok = CGAL_NTS is_finite(sdist);
    }
  }

  return cgal_make_optional(ok,sdist);
}

} // namnepsace CGAIL_SS_i

CGAL_END_NAMESPACE

#endif // CGAL_STRAIGHT_SKELETON_CONS_FTC2_H //
// EOF //

