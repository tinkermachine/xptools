// Copyright (c) 2005  Tel-Aviv University (Israel).
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.3-branch/Arrangement_2/include/CGAL/Arr_circle_segment_traits_2.h $
// $Id$
// 
//
// Author(s)     : Ron Wein          <wein@post.tau.ac.il>
//                 Baruch Zukerman   <baruchzu@post.tau.ac.il>

#ifndef CGAL_ARR_CIRCLE_SEGMENT_TRAITS_2_H
#define CGAL_ARR_CIRCLE_SEGMENT_TRAITS_2_H

/*! \file
 * The header file for the Arr_circle_segment_traits_2<Kenrel> class.
 */

#include <CGAL/tags.h>
#include <CGAL/Arr_traits_2/Circle_segment_2.h>

#include <fstream>

CGAL_BEGIN_NAMESPACE

/*! \class
 * A traits class for maintaining an arrangement of circles.
 */
template <class Kernel_, bool Filter_ = true>
class Arr_circle_segment_traits_2 
{
public:

  typedef Kernel_                                        Kernel;
  typedef typename Kernel::FT                            NT;
  typedef _One_root_point_2<NT, Filter_>                 Point_2;
  typedef typename Point_2::CoordNT                      CoordNT;
  typedef _Circle_segment_2<Kernel, Filter_>             Curve_2;
  typedef _X_monotone_circle_segment_2<Kernel, Filter_>  X_monotone_curve_2;
  typedef Arr_circle_segment_traits_2<Kernel, Filter_>   Self;

  // Category tags:
  typedef Tag_true                                       Has_left_category;
  typedef Tag_true                                       Has_merge_category;
  typedef Tag_false                                      Has_boundary_category;

protected:

  // Type definition for the intersection points mapping.
  typedef typename X_monotone_curve_2::Intersection_map   Intersection_map;

  Intersection_map  inter_map;   // Mapping pairs of curve IDs to their
                                 // intersection points.
  bool m_use_cache;

public:

  /*! Default constructor. */
  Arr_circle_segment_traits_2 (bool use_intersection_caching = false) :
      m_use_cache(use_intersection_caching)
  {}

  /*! Get the next curve index. */
  static unsigned int get_index () 
  {
    static unsigned int index = 0;
    return (++index);
  }

  /// \name Basic functor definitions.
  //@{

  class Compare_x_2
  {
  public:
    /*!
     * Compare the x-coordinates of two points.
     * \param p1 The first point.
     * \param p2 The second point.
     * \return LARGER if x(p1) > x(p2);
     *         SMALLER if x(p1) < x(p2);
     *         EQUAL if x(p1) = x(p2).
     */
    Comparison_result operator() (const Point_2& p1, const Point_2& p2) const
    {
      if (p1.identical (p2))
        return (EQUAL);

      return (CGAL::compare (p1.x(), p2.x()));
    }
  };

  /*! Get a Compare_x_2 functor object. */
  Compare_x_2 compare_x_2_object () const
  {
    return Compare_x_2();
  }

  class Compare_xy_2
  {
  public:
    /*!
     * Compares two points lexigoraphically: by x, then by y.
     * \param p1 The first point.
     * \param p2 The second point.
     * \return LARGER if x(p1) > x(p2), or if x(p1) = x(p2) and y(p1) > y(p2);
     *         SMALLER if x(p1) < x(p2), or if x(p1) = x(p2) and y(p1) < y(p2);
     *         EQUAL if the two points are equal.
     */
    Comparison_result operator() (const Point_2& p1, const Point_2& p2) const
    {
      if (p1.identical (p2))
        return (EQUAL);

      Comparison_result  res = CGAL::compare (p1.x(), p2.x());

      if (res != EQUAL)
        return (res);

      return (CGAL::compare (p1.y(), p2.y()));
    }
  };

  /*! Get a Compare_xy_2 functor object. */
  Compare_xy_2 compare_xy_2_object () const
  {
    return Compare_xy_2();
  }

  class Construct_min_vertex_2
  {
  public:
    /*!
     * Get the left endpoint of the x-monotone curve (segment).
     * \param cv The curve.
     * \return The left endpoint.
     */
    const Point_2& operator() (const X_monotone_curve_2 & cv) const
    {
      return (cv.left());
    }
  };

  /*! Get a Construct_min_vertex_2 functor object. */
  Construct_min_vertex_2 construct_min_vertex_2_object () const
  {
    return Construct_min_vertex_2();
  }

  class Construct_max_vertex_2
  {
  public:
    /*!
     * Get the right endpoint of the x-monotone curve (segment).
     * \param cv The curve.
     * \return The right endpoint.
     */
    const Point_2& operator() (const X_monotone_curve_2 & cv) const
    {
      return (cv.right());
    }
  };

  /*! Get a Construct_max_vertex_2 functor object. */
  Construct_max_vertex_2 construct_max_vertex_2_object () const
  {
    return Construct_max_vertex_2();
  }

  class Is_vertical_2
  {
  public:
    /*!
     * Check whether the given x-monotone curve is a vertical segment.
     * \param cv The curve.
     * \return (true) if the curve is a vertical segment; (false) otherwise.
     */
    bool operator() (const X_monotone_curve_2& cv) const
    {
      return (cv.is_vertical());
    }
  };

  /*! Get an Is_vertical_2 functor object. */
  Is_vertical_2 is_vertical_2_object () const
  {
    return Is_vertical_2();
  }

  class Compare_y_at_x_2
  {
  public:
    /*!
     * Return the location of the given point with respect to the input curve.
     * \param cv The curve.
     * \param p The point.
     * \pre p is in the x-range of cv.
     * \return SMALLER if y(p) < cv(x(p)), i.e. the point is below the curve;
     *         LARGER if y(p) > cv(x(p)), i.e. the point is above the curve;
     *         EQUAL if p lies on the curve.
     */
    Comparison_result operator() (const Point_2& p,
                                  const X_monotone_curve_2& cv) const
    {
      CGAL_precondition (cv.is_in_x_range (p));

      return (cv.point_position (p));
    }
  };

  /*! Get a Compare_y_at_x_2 functor object. */
  Compare_y_at_x_2 compare_y_at_x_2_object () const
  {
    return Compare_y_at_x_2();
  }

  class Compare_y_at_x_right_2
  {
  public:
    /*!
     * Compares the y value of two x-monotone curves immediately to the right
     * of their intersection point.
     * \param cv1 The first curve.
     * \param cv2 The second curve.
     * \param p The intersection point.
     * \pre The point p lies on both curves, and both of them must be also be
     *      defined (lexicographically) to its right.
     * \return The relative position of cv1 with respect to cv2 immdiately to
     *         the right of p: SMALLER, LARGER or EQUAL.
     */
    Comparison_result operator() (const X_monotone_curve_2& cv1,
                                  const X_monotone_curve_2& cv2,
                                  const Point_2& p) const
    {
      // Make sure that p lies on both curves, and that both are defined to its
      // right (so their right endpoint is lexicographically larger than p).
      CGAL_precondition (cv1.point_position (p) == EQUAL &&
                         cv2.point_position (p) == EQUAL);

      CGAL_precondition (CGAL::compare (p.x(), cv1.right().x()) != LARGER &&
                         CGAL::compare (p.x(), cv2.right().x()) != LARGER);

      // Compare the two curves immediately to the right of p:
      return (cv1.compare_to_right (cv2, p));
    }
  };

  /*! Get a Compare_y_at_x_right_2 functor object. */
  Compare_y_at_x_right_2 compare_y_at_x_right_2_object () const
  {
    return Compare_y_at_x_right_2();
  }

  class Compare_y_at_x_left_2
  {
  public:
    /*!
     * Compares the y value of two x-monotone curves immediately to the left
     * of their intersection point.
     * \param cv1 The first curve.
     * \param cv2 The second curve.
     * \param p The intersection point.
     * \pre The point p lies on both curves, and both of them must be also be
     *      defined (lexicographically) to its left.
     * \return The relative position of cv1 with respect to cv2 immdiately to
     *         the left of p: SMALLER, LARGER or EQUAL.
     */
    Comparison_result operator() (const X_monotone_curve_2& cv1,
                                  const X_monotone_curve_2& cv2,
                                  const Point_2& p) const
    {
      // Make sure that p lies on both curves, and that both are defined to its
      // left (so their left endpoint is lexicographically smaller than p).
      CGAL_precondition (cv1.point_position (p) == EQUAL &&
                         cv2.point_position (p) == EQUAL);

      CGAL_precondition (CGAL::compare (p.x(), cv1.left().x()) != SMALLER &&
                         CGAL::compare (p.x(), cv2.left().x()) != SMALLER);

      // Compare the two curves immediately to the left of p:
      return (cv1.compare_to_left (cv2, p));
    }
  };

  /*! Get a Compare_y_at_x_left_2 functor object. */
  Compare_y_at_x_left_2 compare_y_at_x_left_2_object () const
  {
    return Compare_y_at_x_left_2();
  }

  class Equal_2
  {
  public:
    /*!
     * Check if the two x-monotone curves are the same (have the same graph).
     * \param cv1 The first curve.
     * \param cv2 The second curve.
     * \return (true) if the two curves are the same; (false) otherwise.
     */
    bool operator() (const X_monotone_curve_2& cv1,
                     const X_monotone_curve_2& cv2) const
    {
      if (&cv1 == &cv2)
        return (true);

      return (cv1.equals (cv2));
    }

    /*!
     * Check if the two points are the same.
     * \param p1 The first point.
     * \param p2 The second point.
     * \return (true) if the two point are the same; (false) otherwise.
     */
    bool operator() (const Point_2& p1, const Point_2& p2) const
    {
      return (p1.equals (p2));
    }
  };

  /*! Get an Equal_2 functor object. */
  Equal_2 equal_2_object () const
  {
    return Equal_2();
  }
  //@}

  /// \name Functor definitions for supporting intersections.
  //@{

  class Make_x_monotone_2
  {
  private:
    typedef Arr_circle_segment_traits_2<Kernel_, Filter_> Self;

    bool m_use_cache;

  public:

    Make_x_monotone_2(bool use_cache = false) : m_use_cache(use_cache)
    {}

    /*!
     * Cut the given conic curve (or conic arc) into x-monotone subcurves 
     * and insert them to the given output iterator.
     * \param cv The curve.
     * \param oi The output iterator, whose value-type is Object. The returned
     *           objects are all wrappers X_monotone_curve_2 objects.
     * \return The past-the-end iterator.
     */
    template<class OutputIterator>
    OutputIterator operator() (const Curve_2& cv, OutputIterator oi)
    {
      // Increment the serial number of the curve cv, which will serve as its
      // unique identifier.
      unsigned int  index = 0;
      if(m_use_cache)
        index = Self::get_index();

      if (cv.orientation() == COLLINEAR)
      {
        // The curve is a line segment.
        *oi = make_object (X_monotone_curve_2 (cv.supporting_line(),
                                               cv.source(), cv.target(),
                                               index));
        ++oi;
        return (oi);
      }

      // Check the case of a degenrate circle (a point).
      const typename Kernel::Circle_2&  circ = cv.supporting_circle();
      CGAL::Sign   sign_rad = CGAL::sign (circ.squared_radius());
      CGAL_precondition (sign_rad != NEGATIVE);
      
      if (sign_rad == ZERO)
      {
        // Create an isolated point.
        *oi = make_object (Point_2 (circ.center().x(), circ.center().y()));
        ++oi;
        return (oi);
      }
      
      // The curve is circular: compute the to vertical tangency points
      // of the supporting circle.
      Point_2         vpts[2];
      unsigned int    n_vpts = cv.vertical_tangency_points (vpts);

      if (cv.is_full())
      {
        CGAL_assertion (n_vpts == 2);

        // Subdivide the circle into two arcs (an upper and a lower half).
        *oi = make_object (X_monotone_curve_2 (circ,
                                               vpts[0], vpts[1],
                                               cv.orientation(),
                                               index));
        ++oi;
        
        *oi = make_object (X_monotone_curve_2 (circ,
                                               vpts[1], vpts[0],
                                               cv.orientation(),
                                               index));
        ++oi;
      }
      else
      {
        // Act according to the number of vertical tangency points.
        if (n_vpts == 2)
        {
          // Subdivide the circular arc into three x-monotone arcs.
          *oi = make_object (X_monotone_curve_2 (circ,
                                                 cv.source(), vpts[0],
                                                 cv.orientation(),
                                                 index));
          ++oi;
        
          *oi = make_object (X_monotone_curve_2 (circ,
                                                 vpts[0], vpts[1],
                                                 cv.orientation(),
                                                 index));
          ++oi;

          *oi = make_object (X_monotone_curve_2 (circ,
                                                 vpts[1], cv.target(),
                                                 cv.orientation(),
                                                 index));
          ++oi;
        }
        else if (n_vpts == 1)
        {
          // Subdivide the circular arc into two x-monotone arcs.
          *oi = make_object (X_monotone_curve_2 (circ,
                                                 cv.source(), vpts[0],
                                                 cv.orientation(),
                                                 index));
          ++oi;
        
          *oi = make_object (X_monotone_curve_2 (circ,
                                                 vpts[0], cv.target(),
                                                 cv.orientation(),
                                                 index));
          ++oi;
        }
        else
        {
          CGAL_assertion (n_vpts == 0);

          // The arc is already x-monotone:
          *oi = make_object (X_monotone_curve_2 (circ,
                                                 cv.source(), cv.target(),
                                                 cv.orientation(),
                                                 index));
          ++oi;
        }
      }
      
      return (oi);
    }
  };

  /*! Get a Make_x_monotone_2 functor object. */
  Make_x_monotone_2 make_x_monotone_2_object ()
  {
    return Make_x_monotone_2(m_use_cache);
  }

  class Split_2
  {
  public:

    /*!
     * Split a given x-monotone curve at a given point into two sub-curves.
     * \param cv The curve to split
     * \param p The split point.
     * \param c1 Output: The left resulting subcurve (p is its right endpoint).
     * \param c2 Output: The right resulting subcurve (p is its left endpoint).
     * \pre p lies on cv but is not one of its end-points.
     */
    void operator() (const X_monotone_curve_2& cv, const Point_2& p,
                     X_monotone_curve_2& c1, X_monotone_curve_2& c2) const
    {
      CGAL_precondition (cv.is_in_x_range (p) &&
                         ! p.equals (cv.source()) && ! p.equals (cv.target()));

      cv.split (p, c1, c2);
      return;
    }
  };

  /*! Get a Split_2 functor object. */
  Split_2 split_2_object ()
  {
    return Split_2();
  }

  class Intersect_2
  {
  private:

    Intersection_map&  _inter_map;       // The map of intersection points.

  public:

    /*! Constructor. */
    Intersect_2 (Intersection_map& map) :
      _inter_map (map)
    {}

    /*!
     * Find the intersections of the two given curves and insert them to the
     * given output iterator. As two segments may itersect only once, only a
     * single will be contained in the iterator.
     * \param cv1 The first curve.
     * \param cv2 The second curve.
     * \param oi The output iterator.
     * \return The past-the-end iterator.
     */
    template<class OutputIterator>
    OutputIterator operator() (const X_monotone_curve_2& cv1,
                               const X_monotone_curve_2& cv2,
                               OutputIterator oi)
    {
      return (cv1.intersect (cv2, oi, &_inter_map));
    }
  };

  /*! Get an Intersect_2 functor object. */
  Intersect_2 intersect_2_object ()
  {
    return (Intersect_2 (inter_map));
  }

  class Are_mergeable_2
  {
  public:
    /*!
     * Check whether it is possible to merge two given x-monotone curves.
     * \param cv1 The first curve.
     * \param cv2 The second curve.
     * \return (true) if the two curves are mergeable - if they are supported
     *         by the same line and share a common endpoint; (false) otherwise.
     */
    bool operator() (const X_monotone_curve_2& cv1,
                     const X_monotone_curve_2& cv2) const
    {
      return (cv1.can_merge_with (cv2));
    }
  };

  /*! Get an Are_mergeable_2 functor object. */
  Are_mergeable_2 are_mergeable_2_object () const
  {
    return Are_mergeable_2();
  }

  class Merge_2
  {
  public:
    /*!
     * Merge two given x-monotone curves into a single curve.
     * \param cv1 The first curve.
     * \param cv2 The second curve.
     * \param c Output: The merged curve.
     * \pre The two curves are mergeable, that is they are supported by the
     *      same conic curve and share a common endpoint.
     */
    void operator() (const X_monotone_curve_2& cv1,
                     const X_monotone_curve_2& cv2,
                     X_monotone_curve_2& c) const
    {
      c = cv1;
      c.merge (cv2);

      return;
    }
  };

  /*! Get a Merge_2 functor object. */
  Merge_2 merge_2_object () const
  {
    return Merge_2();
  }

  class Compare_endpoints_xy_2
  {
  public:
    /*!
     * compare lexicogrphic the endpoints of a x-monotone curve.
     * \param cv the curve
     * \return SMALLER if the curve is directed right, else return SMALLER
     */
    Comparison_result operator()(const X_monotone_curve_2& cv)
    {
      if(cv.is_directed_right())
        return(SMALLER);
      return (LARGER);
    }
  };

  /*! Get a Compare_endpoints_xy_2 functor object. */
  Compare_endpoints_xy_2 compare_endpoints_xy_2_object() const
  {
    return Compare_endpoints_xy_2();
  }


  class Construct_opposite_2
  {
  public:
    /*!
     * construct an opposite x-monotone curve.
     * \param cv the curve
     * \return an opposite x-monotone curve.
     */
    X_monotone_curve_2 operator()(const X_monotone_curve_2& cv)
    {
      return cv.construct_opposite();
    }
  };

  /*! Get a Construct_opposite_2 functor object. */
  Construct_opposite_2 construct_opposite_2_object() const
  {
    return Construct_opposite_2();
  } 

};

CGAL_END_NAMESPACE

#endif
