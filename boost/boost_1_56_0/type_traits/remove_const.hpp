
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.  
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef BOOST_TT_REMOVE_CONST_HPP_INCLUDED
#define BOOST_TT_REMOVE_CONST_HPP_INCLUDED

#include <boost_1_56_0/type_traits/is_volatile.hpp>
#include <boost_1_56_0/type_traits/detail/cv_traits_impl.hpp>
#include <boost_1_56_0/config.hpp>
#include <boost_1_56_0/detail/workaround.hpp>

#include <cstddef>

// should be the last #include
#include <boost_1_56_0/type_traits/detail/type_trait_def.hpp>

namespace boost_1_56_0 {


namespace detail {

template <typename T, bool is_vol>
struct remove_const_helper
{
    typedef T type;
};

template <typename T>
struct remove_const_helper<T, true>
{
    typedef T volatile type;
};


template <typename T>
struct remove_const_impl
{
    typedef typename remove_const_helper<
       typename cv_traits_imp<BOOST_TT_AUX_CV_TRAITS_IMPL_PARAM(T)>::unqualified_type
        , ::boost_1_56_0::is_volatile<T>::value
        >::type type;
};

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
//
// We can't filter out rvalue_references at the same level as
// references or we get ambiguities from msvc:
//
template <typename T>
struct remove_const_impl<T&&>
{
    typedef T&& type;
};
#endif

} // namespace detail

// * convert a type T to non-const type - remove_const<T>

BOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_const,T,typename boost_1_56_0::detail::remove_const_impl<T>::type)
BOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_const,T&,T&)
#if !defined(BOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
BOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_const,T const[N],T type[N])
BOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_const,T const volatile[N],T volatile type[N])
#endif


} // namespace boost_1_56_0

#include <boost_1_56_0/type_traits/detail/type_trait_undef.hpp>

#endif // BOOST_TT_REMOVE_CONST_HPP_INCLUDED
