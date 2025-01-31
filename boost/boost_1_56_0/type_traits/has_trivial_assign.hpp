
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_HAS_TRIVIAL_ASSIGN_HPP_INCLUDED
#define BOOST_TT_HAS_TRIVIAL_ASSIGN_HPP_INCLUDED

#include <boost_1_56_0/type_traits/config.hpp>
#include <boost_1_56_0/type_traits/intrinsics.hpp>
#include <boost_1_56_0/type_traits/is_pod.hpp>
#include <boost_1_56_0/type_traits/is_const.hpp>
#include <boost_1_56_0/type_traits/is_volatile.hpp>
#include <boost_1_56_0/type_traits/detail/ice_and.hpp>
#include <boost_1_56_0/type_traits/detail/ice_or.hpp>
#include <boost_1_56_0/type_traits/detail/ice_not.hpp>

// should be the last #include
#include <boost_1_56_0/type_traits/detail/bool_trait_def.hpp>

namespace boost_1_56_0 {

namespace detail {

template <typename T>
struct has_trivial_assign_impl
{
#ifdef BOOST_HAS_TRIVIAL_ASSIGN
   BOOST_STATIC_CONSTANT(bool, value = BOOST_HAS_TRIVIAL_ASSIGN(T));
#else
   BOOST_STATIC_CONSTANT(bool, value =
      (::boost_1_56_0::type_traits::ice_and<
        ::boost_1_56_0::is_pod<T>::value,
        ::boost_1_56_0::type_traits::ice_not< ::boost_1_56_0::is_const<T>::value >::value,
      ::boost_1_56_0::type_traits::ice_not< ::boost_1_56_0::is_volatile<T>::value >::value
      >::value));
#endif
};

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF1(has_trivial_assign,T,::boost_1_56_0::detail::has_trivial_assign_impl<T>::value)
BOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_assign,void,false)
#ifndef BOOST_NO_CV_VOID_SPECIALIZATIONS
BOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_assign,void const,false)
BOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_assign,void const volatile,false)
BOOST_TT_AUX_BOOL_TRAIT_SPEC1(has_trivial_assign,void volatile,false)
#endif

} // namespace boost_1_56_0

#include <boost_1_56_0/type_traits/detail/bool_trait_undef.hpp>

#endif // BOOST_TT_HAS_TRIVIAL_ASSIGN_HPP_INCLUDED
