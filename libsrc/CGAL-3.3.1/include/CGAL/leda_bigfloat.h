// Copyright (c) 1999,2007  Utrecht University (The Netherlands),
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.3-branch/Number_types/include/CGAL/leda_bigfloat.h $
// $Id$
//
//
// Author(s)     : Stefan Schirra, Michael Hemmer


#ifndef CGAL_LEDA_BIGFLOAT_H
#define CGAL_LEDA_BIGFLOAT_H

#include <CGAL/number_type_basic.h>

#include <utility>

#include <CGAL/leda_coercion_traits.h>
#include <CGAL/Interval_nt.h>



#include <CGAL/LEDA_basic.h>
#if CGAL_LEDA_VERSION < 500
#include <LEDA/bigfloat.h>
#else
#include <LEDA/numbers/bigfloat.h>
#endif


CGAL_BEGIN_NAMESPACE

template <> class Algebraic_structure_traits< leda_bigfloat >
  : public Algebraic_structure_traits_base< leda_bigfloat,
                                            Field_with_kth_root_tag >  {
  public:
    typedef Tag_false           Is_exact;
    typedef Tag_true            Is_numerical_sensitive;

    class Sqrt
      : public Unary_function< Type, Type > {
      public:
        Type operator()( const Type& x ) const {
          return CGAL_LEDA_SCOPE::sqrt( x );
        }
    };

    class Kth_root
      : public Binary_function<int, Type, Type> {
      public:
        Type operator()( int k,
                                        const Type& x) const {
            CGAL_precondition_msg(k > 0, "'k' must be positive for k-th roots");
            // heuristic: we ask for as many precision as the argument has
            long d = x.get_significant_length();
            if ( d < 53) // O.K. we want at least double precision
                d = 53;
            return CGAL_LEDA_SCOPE::sqrt_d( x, d, k);
        }
    };

};

template <> class Real_embeddable_traits< leda_bigfloat >
  : public Real_embeddable_traits_base< leda_bigfloat > {
  public:

    class Abs
      : public Unary_function< Type, Type > {
      public:
        Type operator()( const Type& x ) const {
            return CGAL_LEDA_SCOPE::abs( x );
        }
    };

    class Sign
      : public Unary_function< Type, ::CGAL::Sign > {
      public:
        ::CGAL::Sign operator()( const Type& x ) const {
          return (::CGAL::Sign) CGAL_LEDA_SCOPE::sign( x );
        }
    };

    class Compare
      : public Binary_function< Type, Type,
                                Comparison_result > {
      public:
        Comparison_result operator()( const Type& x,
                                            const Type& y ) const {
          return (Comparison_result) CGAL_LEDA_SCOPE::compare( x, y );
        }
    };

    class To_double
      : public Unary_function< Type, double > {
      public:
        double operator()( const Type& x ) const {
          return x.to_double();
        }
    };

    class To_interval
      : public Unary_function< Type, std::pair< double, double > > {
      public:
        std::pair<double, double> operator()( const Type& x ) const {

          // assuming leda_bigfloat guarantee 1 bit error max
          Protect_FPU_rounding<true> P (CGAL_FE_TONEAREST);
          Interval_nt_advanced approx (CGAL_LEDA_SCOPE::to_double(x));
          FPU_set_cw(CGAL_FE_UPWARD);
          approx += Interval_nt<false>::smallest();
          return approx.pair();
        }
    };

    class Is_finite
      : public Unary_function< Type, bool > {
      public:
        bool operator()( const Type& x )  const {
          return !( CGAL_LEDA_SCOPE::isInf(x) || CGAL_LEDA_SCOPE::isNaN(x) );
        }
    };
};

template<>
class Is_valid< leda_bigfloat >
  : public Unary_function< leda_bigfloat, bool > {
  public :
    bool operator()( const leda_bigfloat& x ) const {
      return !( CGAL_LEDA_SCOPE::isNaN(x) );
    }
};

CGAL_END_NAMESPACE

// Unary + is missing for leda::bigfloat
namespace leda {
    inline bigfloat operator+( const bigfloat& i) { return i; }
} // namespace leda

//since types are included by leda_coercion_traits.h:
#include <CGAL/leda_integer.h>
#include <CGAL/leda_rational.h>
#include <CGAL/leda_bigfloat.h>
#include <CGAL/leda_real.h>

#endif // CGAL_LEDA_BIGFLOAT_H
