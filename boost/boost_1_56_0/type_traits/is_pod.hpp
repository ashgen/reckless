
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_IS_POD_HPP_INCLUDED
#define BOOST_TT_IS_POD_HPP_INCLUDED

#include <boost_1_56_0/type_traits/config.hpp>
#include <boost_1_56_0/type_traits/is_void.hpp>
#include <boost_1_56_0/type_traits/is_scalar.hpp>
#include <boost_1_56_0/type_traits/detail/ice_or.hpp>
#include <boost_1_56_0/type_traits/intrinsics.hpp>

#include <cstddef>

// should be the last #include
#include <boost_1_56_0/type_traits/detail/bool_trait_def.hpp>

#ifndef BOOST_IS_POD
#define BOOST_INTERNAL_IS_POD(T) false
#else
#define BOOST_INTERNAL_IS_POD(T) BOOST_IS_POD(T)
#endif

namespace boost_1_56_0 {

// forward declaration, needed by 'is_pod_array_helper' template below
template< typename T > struct is_POD;

namespace detail {


template <typename T> struct is_pod_impl
{ 
    BOOST_STATIC_CONSTANT(
        bool, value =
        (::boost_1_56_0::type_traits::ice_or<
            ::boost_1_56_0::is_scalar<T>::value,
            ::boost_1_56_0::is_void<T>::value,
            BOOST_INTERNAL_IS_POD(T)
         >::value));
};

#if !defined(BOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
template <typename T, std::size_t sz>
struct is_pod_impl<T[sz]>
    : public is_pod_impl<T>
{
};
#endif


// the following help compilers without partial specialization support:
BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void,true)

#ifndef BOOST_NO_CV_VOID_SPECIALIZATIONS
BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void const,true)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void volatile,true)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void const volatile,true)
#endif

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_pod,T,::boost_1_56_0::detail::is_pod_impl<T>::value)
// is_POD is the old depricated name for this trait, do not use this as it may
// be removed in future without warning!!
BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_POD,T,::boost_1_56_0::is_pod<T>::value)

} // namespace boost_1_56_0

#include <boost_1_56_0/type_traits/detail/bool_trait_undef.hpp>

#undef BOOST_INTERNAL_IS_POD

#endif // BOOST_TT_IS_POD_HPP_INCLUDED
