
// Copyright 2000 John Maddock (john@johnmaddock.co.uk)
// Copyright 2000 Jeremy Siek (jsiek@lsc.nd.edu)
// Copyright 1999, 2000 Jaakko Jarvi (jaakko.jarvi@cs.utu.fi)
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_IS_CONVERTIBLE_HPP_INCLUDED
#define BOOST_TT_IS_CONVERTIBLE_HPP_INCLUDED

#include <boost_1_56_0/type_traits/intrinsics.hpp>
#ifndef BOOST_IS_CONVERTIBLE
#include <boost_1_56_0/type_traits/detail/yes_no_type.hpp>
#include <boost_1_56_0/type_traits/config.hpp>
#include <boost_1_56_0/type_traits/is_array.hpp>
#include <boost_1_56_0/type_traits/ice.hpp>
#include <boost_1_56_0/type_traits/is_arithmetic.hpp>
#include <boost_1_56_0/type_traits/is_void.hpp>
#ifndef BOOST_NO_IS_ABSTRACT
#include <boost_1_56_0/type_traits/is_abstract.hpp>
#endif
#include <boost_1_56_0/type_traits/add_lvalue_reference.hpp>
#include <boost_1_56_0/type_traits/add_rvalue_reference.hpp>
#include <boost_1_56_0/type_traits/is_function.hpp>

#if defined(__MWERKS__)
#include <boost_1_56_0/type_traits/remove_reference.hpp>
#endif
#if !defined(BOOST_NO_SFINAE_EXPR) && !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#  include <boost_1_56_0/utility/declval.hpp>
#endif
#endif // BOOST_IS_CONVERTIBLE

// should be always the last #include directive
#include <boost_1_56_0/type_traits/detail/bool_trait_def.hpp>

namespace boost_1_56_0 {

#ifndef BOOST_IS_CONVERTIBLE

// is one type convertible to another?
//
// there are multiple versions of the is_convertible
// template, almost every compiler seems to require its
// own version.
//
// Thanks to Andrei Alexandrescu for the original version of the
// conversion detection technique!
//

namespace detail {

#if !defined(BOOST_NO_SFINAE_EXPR) && !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

   // This is a C++11 conforming version, place this first and use it wherever possible:

#  define BOOST_TT_CXX11_IS_CONVERTIBLE

   template <class A, class B, class C>
   struct or_helper
   {
      static const bool value = (A::value || B::value || C::value);
   };

   template<typename From, typename To, bool b = or_helper<boost_1_56_0::is_void<From>, boost_1_56_0::is_function<To>, boost_1_56_0::is_array<To> >::value>
   struct is_convertible_basic_impl
   {
      // Nothing converts to function or array, but void converts to void:
      static const bool value = is_void<To>::value; 
   };

   template<typename From, typename To>
   class is_convertible_basic_impl<From, To, false>
   {
      typedef char one;
      typedef int  two;

      template<typename To1>
      static void test_aux(To1);

      template<typename From1, typename To1>
      static decltype(test_aux<To1>(boost_1_56_0::declval<From1>()), one()) test(int);

      template<typename, typename>
      static two test(...);

   public:
      static const bool value = sizeof(test<From, To>(0)) == 1;
   };

#elif defined(__BORLANDC__) && (__BORLANDC__ < 0x560)
//
// special version for Borland compilers
// this version breaks when used for some
// UDT conversions:
//
template <typename From, typename To>
struct is_convertible_impl
{
#pragma option push -w-8074
    // This workaround for Borland breaks the EDG C++ frontend,
    // so we only use it for Borland.
    template <typename T> struct checker
    {
        static ::boost_1_56_0::type_traits::no_type BOOST_TT_DECL _m_check(...);
        static ::boost_1_56_0::type_traits::yes_type BOOST_TT_DECL _m_check(T);
    };

    static typename add_lvalue_reference<From>::type  _m_from;
    static bool const value = sizeof( checker<To>::_m_check(_m_from) )
        == sizeof(::boost_1_56_0::type_traits::yes_type);
#pragma option pop
};

#elif defined(__GNUC__) || defined(__BORLANDC__) && (__BORLANDC__ < 0x600)
// special version for gcc compiler + recent Borland versions
// note that this does not pass UDT's through (...)

struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(const T&);
    template <typename T> any_conversion(volatile T&);
    template <typename T> any_conversion(T&);
};

template <typename T> struct checker
{
    static boost_1_56_0::type_traits::no_type _m_check(any_conversion ...);
    static boost_1_56_0::type_traits::yes_type _m_check(T, int);
};

template <typename From, typename To>
struct is_convertible_basic_impl
{
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type;
    static lvalue_type _m_from;
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 6)))
    static bool const value =
        sizeof( boost_1_56_0::detail::checker<To>::_m_check(static_cast<rvalue_type>(_m_from), 0) )
        == sizeof(::boost_1_56_0::type_traits::yes_type);
#else
    static bool const value =
        sizeof( boost_1_56_0::detail::checker<To>::_m_check(_m_from, 0) )
        == sizeof(::boost_1_56_0::type_traits::yes_type);
#endif
};

#elif (defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 245) && !defined(__ICL)) \
      || defined(__IBMCPP__) || defined(__HP_aCC)
//
// This is *almost* an ideal world implementation as it doesn't rely
// on undefined behaviour by passing UDT's through (...).
// Unfortunately it doesn't quite pass all the tests for most compilers (sigh...)
// Enable this for your compiler if is_convertible_test.cpp will compile it...
//
// Note we do not enable this for VC7.1, because even though it passes all the
// type_traits tests it is known to cause problems when instantiation occurs
// deep within the instantiation tree :-(
//
struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(const T&);
    template <typename T> any_conversion(volatile T&);
    // we need this constructor to catch references to functions
    // (which can not be cv-qualified):
    template <typename T> any_conversion(T&);
};

template <typename From, typename To>
struct is_convertible_basic_impl
{
    static ::boost_1_56_0::type_traits::no_type BOOST_TT_DECL _m_check(any_conversion ...);
    static ::boost_1_56_0::type_traits::yes_type BOOST_TT_DECL _m_check(To, int);
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type; 
    static lvalue_type _m_from;

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    BOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(static_cast<rvalue_type>(_m_from), 0) ) == sizeof(::boost_1_56_0::type_traits::yes_type)
        );
#else
    BOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(_m_from, 0) ) == sizeof(::boost_1_56_0::type_traits::yes_type)
        );
#endif
};

#elif defined(__DMC__)

struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(const T&);
    template <typename T> any_conversion(volatile T&);
    // we need this constructor to catch references to functions
    // (which can not be cv-qualified):
    template <typename T> any_conversion(T&);
};

template <typename From, typename To>
struct is_convertible_basic_impl
{
    // Using '...' doesn't always work on Digital Mars. This version seems to.
    template <class T>
    static ::boost_1_56_0::type_traits::no_type BOOST_TT_DECL _m_check(any_conversion,  float, T);
    static ::boost_1_56_0::type_traits::yes_type BOOST_TT_DECL _m_check(To, int, int);
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type;
    static lvalue_type _m_from;

    // Static constants sometime cause the conversion of _m_from to To to be
    // called. This doesn't happen with an enum.
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    enum { value =
        sizeof( _m_check(static_cast<rvalue_type>(_m_from), 0, 0) ) == sizeof(::boost_1_56_0::type_traits::yes_type)
        };
#else
    enum { value =
        sizeof( _m_check(_m_from, 0, 0) ) == sizeof(::boost_1_56_0::type_traits::yes_type)
        };
#endif
};

#elif defined(__MWERKS__)
// 
// CW works with the technique implemented above for EDG, except when From
// is a function type (or a reference to such a type), in which case
// any_conversion won't be accepted as a valid conversion. We detect this
// exceptional situation and channel it through an alternative algorithm.
//

template <typename From, typename To,bool FromIsFunctionRef>
struct is_convertible_basic_impl_aux;

struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(const T&);
    template <typename T> any_conversion(volatile T&);
    template <typename T> any_conversion(T&);
};

template <typename From, typename To>
struct is_convertible_basic_impl_aux<From,To,false /*FromIsFunctionRef*/>
{
    static ::boost_1_56_0::type_traits::no_type BOOST_TT_DECL _m_check(any_conversion ...);
    static ::boost_1_56_0::type_traits::yes_type BOOST_TT_DECL _m_check(To, int);
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type; 
    static lvalue_type _m_from;

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    BOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(static_cast<rvalue_type>(_m_from), 0) ) == sizeof(::boost_1_56_0::type_traits::yes_type)
        );
#else
    BOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(_m_from, 0) ) == sizeof(::boost_1_56_0::type_traits::yes_type)
        );
#endif
};

template <typename From, typename To>
struct is_convertible_basic_impl_aux<From,To,true /*FromIsFunctionRef*/>
{
    static ::boost_1_56_0::type_traits::no_type BOOST_TT_DECL _m_check(...);
    static ::boost_1_56_0::type_traits::yes_type BOOST_TT_DECL _m_check(To);
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type;
    static lvalue_type _m_from;
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    BOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(static_cast<rvalue_type>(_m_from)) ) == sizeof(::boost_1_56_0::type_traits::yes_type)
        );
#else
    BOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(_m_from) ) == sizeof(::boost_1_56_0::type_traits::yes_type)
        );
#endif
};

template <typename From, typename To>
struct is_convertible_basic_impl:
  is_convertible_basic_impl_aux<
    From,To,
    ::boost_1_56_0::is_function<typename ::boost_1_56_0::remove_reference<From>::type>::value
  >
{};

#else
//
// This version seems to work pretty well for a wide spectrum of compilers,
// however it does rely on undefined behaviour by passing UDT's through (...).
//
template <typename From, typename To>
struct is_convertible_basic_impl
{
    static ::boost_1_56_0::type_traits::no_type BOOST_TT_DECL _m_check(...);
    static ::boost_1_56_0::type_traits::yes_type BOOST_TT_DECL _m_check(To);
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type; 
    static lvalue_type _m_from;
#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4244)
#if BOOST_WORKAROUND(BOOST_MSVC_FULL_VER, >= 140050000)
#pragma warning(disable:6334)
#endif
#endif
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    BOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(static_cast<rvalue_type>(_m_from)) ) == sizeof(::boost_1_56_0::type_traits::yes_type)
        );
#else
    BOOST_STATIC_CONSTANT(bool, value =
        sizeof( _m_check(_m_from) ) == sizeof(::boost_1_56_0::type_traits::yes_type)
        );
#endif
#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
};

#endif // is_convertible_impl

#if defined(__DMC__)
// As before, a static constant sometimes causes errors on Digital Mars.
template <typename From, typename To>
struct is_convertible_impl
{
    enum { value =
        (::boost_1_56_0::type_traits::ice_and<
            ::boost_1_56_0::type_traits::ice_or<
               ::boost_1_56_0::detail::is_convertible_basic_impl<From,To>::value,
               ::boost_1_56_0::is_void<To>::value
            >::value,
            ::boost_1_56_0::type_traits::ice_not<
               ::boost_1_56_0::is_array<To>::value
            >::value,
            ::boost_1_56_0::type_traits::ice_not<
               ::boost_1_56_0::is_function<To>::value
            >::value
        >::value) };
};
#elif !defined(__BORLANDC__) || __BORLANDC__ > 0x551
template <typename From, typename To>
struct is_convertible_impl
{
    BOOST_STATIC_CONSTANT(bool, value =
        (::boost_1_56_0::type_traits::ice_and<
            ::boost_1_56_0::type_traits::ice_or<
               ::boost_1_56_0::detail::is_convertible_basic_impl<From,To>::value,
               ::boost_1_56_0::is_void<To>::value
            >::value,
            ::boost_1_56_0::type_traits::ice_not<
               ::boost_1_56_0::is_array<To>::value
            >::value,
            ::boost_1_56_0::type_traits::ice_not<
               ::boost_1_56_0::is_function<To>::value
            >::value
        >::value)
        );
};
#endif

template <bool trivial1, bool trivial2, bool abstract_target>
struct is_convertible_impl_select
{
   template <class From, class To>
   struct rebind
   {
      typedef is_convertible_impl<From, To> type;
   };
};

template <>
struct is_convertible_impl_select<true, true, false>
{
   template <class From, class To>
   struct rebind
   {
      typedef true_type type;
   };
};

template <>
struct is_convertible_impl_select<false, false, true>
{
   template <class From, class To>
   struct rebind
   {
      typedef false_type type;
   };
};

template <>
struct is_convertible_impl_select<true, false, true>
{
   template <class From, class To>
   struct rebind
   {
      typedef false_type type;
   };
};

template <typename From, typename To>
struct is_convertible_impl_dispatch_base
{
#if !BOOST_WORKAROUND(__HP_aCC, < 60700)
   typedef is_convertible_impl_select< 
      ::boost_1_56_0::is_arithmetic<From>::value, 
      ::boost_1_56_0::is_arithmetic<To>::value,
#if !defined(BOOST_NO_IS_ABSTRACT) && !defined(BOOST_TT_CXX11_IS_CONVERTIBLE)
      // We need to filter out abstract types, only if we don't have a strictly conforming C++11 version:
      ::boost_1_56_0::is_abstract<To>::value
#else
      false
#endif
   > selector;
#else
   typedef is_convertible_impl_select<false, false, false> selector;
#endif
   typedef typename selector::template rebind<From, To> isc_binder;
   typedef typename isc_binder::type type;
};

template <typename From, typename To>
struct is_convertible_impl_dispatch 
   : public is_convertible_impl_dispatch_base<From, To>::type
{};

//
// Now add the full and partial specialisations
// for void types, these are common to all the
// implementation above:
//
#ifndef BOOST_NO_CV_VOID_SPECIALIZATIONS
#   define TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1,spec2,value) \
    BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2,value) \
    BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2 const,value) \
    BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2 volatile,value) \
    BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(trait,spec1,spec2 const volatile,value) \
    /**/

#   define TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2(trait,spec1,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1 const,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1 volatile,spec2,value) \
    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1(trait,spec1 const volatile,spec2,value) \
    /**/

    TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2(is_convertible,void,void,true)

#   undef TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2
#   undef TT_AUX_BOOL_CV_VOID_TRAIT_SPEC2_PART1

#else
    BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC2(is_convertible,void,void,true)
#endif // BOOST_NO_CV_VOID_SPECIALIZATIONS

BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void,To,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void,false)
#ifndef BOOST_NO_CV_VOID_SPECIALIZATIONS
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void const,To,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void volatile,To,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename To,is_convertible,void const volatile,To,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void const,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void volatile,false)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_PARTIAL_SPEC2_1(typename From,is_convertible,From,void const volatile,false)
#endif

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF2(is_convertible,From,To,(::boost_1_56_0::detail::is_convertible_impl_dispatch<From,To>::value))

#else

BOOST_TT_AUX_BOOL_TRAIT_DEF2(is_convertible,From,To,BOOST_IS_CONVERTIBLE(From,To))

#endif

} // namespace boost_1_56_0

#include <boost_1_56_0/type_traits/detail/bool_trait_undef.hpp>

#endif // BOOST_TT_IS_CONVERTIBLE_HPP_INCLUDED
