// Copyright (c) 2006-2007 Max-Planck-Institute Saarbruecken (Germany),
// INRIA Sophia-Antipolis (France).
// All rights reserved.
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.3-branch/Number_types/include/CGAL/Gmpz.h $
// $Id$
//
//
// Author(s)     : Michael Hemmer   <hemmer@mpi-inf.mpg.de>
//                 Sylvain Pion

#ifndef CGAL_GMPZ_H
#define CGAL_GMPZ_H

#include <CGAL/config.h>
#if defined(BOOST_MSVC)
#  pragma warning(push)
#  pragma warning(disable:4800) // complaint about performance in std::map where we can't do anything
#endif

#include <CGAL/number_type_basic.h>
#include <CGAL/Gmp_coercion_traits.h>
#include <CGAL/Quotient.h> // spec of AST for Quotient<Gmpz>

#include <string>
#ifndef CGAL_CFG_NO_LOCALE
#  include <locale>
#else
#  include <cctype>
#endif

#include <CGAL/Root_of_traits.h>

CGAL_BEGIN_NAMESPACE

class Gmpq;

template<>
struct Root_of_traits<Gmpz>: public CGALi::Root_of_traits_helper<Gmpz,
    Euclidean_ring_tag>{
  typedef Gmpq RootOf_1;
  typedef Gmpq Root_of_1;
};


// Algebraic structure traits
template <> class Algebraic_structure_traits< Gmpz >
    : public Algebraic_structure_traits_base< Gmpz,
                                            Euclidean_ring_tag >  {
public:
    typedef Tag_true            Is_exact;
    typedef Tag_false           Is_numerical_sensitive;

    typedef INTERN_AST::Is_square_per_sqrt< Type >
    Is_square;
    class Integral_division
        : public Binary_function< Type, Type,
                                Type > {
    public:
        Type operator()( const Type& x,
                const Type& y ) const {
            Gmpz result;
            mpz_divexact(result.mpz(), x.mpz(), y.mpz());
            CGAL_postcondition_msg(result * y == x, "exact_division failed\n");
            return result;
        }
    };

    class Gcd
        : public Binary_function< Type, Type,
                                Type > {
    public:
        Type operator()( const Type& x,
                const Type& y ) const {
            Gmpz result;
            mpz_gcd(result.mpz(), x.mpz(), y.mpz());
            return result;

        }

        Type operator()( const Type& x,
                                        const int& y ) const {
          if (y > 0)
          {
              Gmpz Res;
              mpz_gcd_ui(Res.mpz(), x.mpz(), y);
              return Res;
          }
          return CGAL_NTS gcd(x, Gmpz(y));
        }

        Type operator()( const int& x,
                                        const Type& y ) const {
          return CGAL_NTS gcd(Gmpz(x), y );
        }
    };

    typedef INTERN_AST::Div_per_operator< Type > Div;
    typedef INTERN_AST::Mod_per_operator< Type > Mod;

    class Sqrt
        : public Unary_function< Type, Type > {
    public:
        Type operator()( const Type& x ) const {
            Gmpz result;
            mpz_sqrt(result.mpz(), x.mpz());
            return result;
        }
    };
};

template <> class Real_embeddable_traits< Gmpz >
    : public Real_embeddable_traits_base< Gmpz > {
public:

    class Sign
        : public Unary_function< Type, ::CGAL::Sign > {
    public:
        ::CGAL::Sign operator()( const Type& x ) const {
            return x.sign();
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

            mpfr_t y;
            mpfr_init2 (y, 53); /* Assume IEEE-754 */
            mpfr_set_z (y, x.mpz(), GMP_RNDD);
            double i = mpfr_get_d (y, GMP_RNDD); /* EXACT but can overflow */
            mpfr_set_z (y, x.mpz(), GMP_RNDU);
            double s = mpfr_get_d (y, GMP_RNDU); /* EXACT but can overflow */
            mpfr_clear (y);
            return std::pair<double, double>(i, s);
        }
    };
};

template<> class Algebraic_structure_traits< Quotient<Gmpz> >
    : public INTERN_QUOTIENT::Algebraic_structure_traits_quotient_base<Quotient<Gmpz> >{
    // specialization of to double functor
public:
    typedef Quotient<Gmpz> Type;

    struct To_double: public Unary_function<Quotient<Gmpz>, double>{
        double operator()(const Quotient<Gmpz>& quot){
            mpq_t  mpQ;
            mpq_init(mpQ);
            const Gmpz& n = quot.numerator();
            const Gmpz& d = quot.denominator();
            mpz_set(mpq_numref(mpQ), n.mpz());
            mpz_set(mpq_denref(mpQ), d.mpz());

            mpq_canonicalize(mpQ);

            double ret = mpq_get_d(mpQ);
            mpq_clear(mpQ);
            return ret;
        }
    };
};

//
// Needs_parens_as_product
//
template <>
struct Needs_parens_as_product<Gmpz> {
  bool operator()(const Gmpz& x) {
    return CGAL_NTS is_negative(x);
  }
};

CGAL_END_NAMESPACE

#if defined(BOOST_MSVC)
#  pragma warning(pop)
#endif


//since types are included by Gmp_coercion_traits.h:
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>
#include <CGAL/Gmpzf.h>

#endif // CGAL_GMPZ_H
