//  Copyright (C) 2011-2013 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_LOCKFREE_DETAIL_ATOMIC_HPP
#define BOOST_LOCKFREE_DETAIL_ATOMIC_HPP

#include <boost_1_56_0/config.hpp>

#ifndef BOOST_LOCKFREE_FORCE_STD_ATOMIC

#define BOOST_LOCKFREE_NO_HDR_ATOMIC

// MSVC supports atomic<> from version 2012 onwards.
#if defined(BOOST_MSVC) && (BOOST_MSVC >= 1700)
#undef BOOST_LOCKFREE_NO_HDR_ATOMIC
#endif

// GCC supports atomic<> from version 4.8 onwards.
#if (BOOST_GCC >= 40800) && (__cplusplus >= 201103L)
#undef BOOST_LOCKFREE_NO_HDR_ATOMIC
#endif

#endif // BOOST_LOCKFREE_FORCE_STD_ATOMIC


#if defined(BOOST_LOCKFREE_NO_HDR_ATOMIC)
#include <boost_1_56_0/atomic.hpp>
#else
#include <atomic>
#endif

namespace boost_1_56_0 {
namespace lockfree {
namespace detail {

#if defined(BOOST_LOCKFREE_NO_HDR_ATOMIC)
using boost_1_56_0::atomic;
using boost_1_56_0::memory_order_acquire;
using boost_1_56_0::memory_order_consume;
using boost_1_56_0::memory_order_relaxed;
using boost_1_56_0::memory_order_release;
#else
using std::atomic;
using std::memory_order_acquire;
using std::memory_order_consume;
using std::memory_order_relaxed;
using std::memory_order_release;
#endif

}
using detail::atomic;
using detail::memory_order_acquire;
using detail::memory_order_consume;
using detail::memory_order_relaxed;
using detail::memory_order_release;

}}

#endif /* BOOST_LOCKFREE_DETAIL_ATOMIC_HPP */
