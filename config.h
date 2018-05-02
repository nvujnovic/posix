#pragma once

//          Copyright Nebojsa Vujnovic 2018 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)


#ifdef _MSC_VER
#  define CP_BREAKPOINT __debugbreak()
#  define CP_LIKELY(x)    (!!(x))
#  define CP_UNLIKELY(x)  (!!(x))
# ifdef NDEBUG
#   define CP_FORCE_INLINE __forceinline 
# else 
#   define CP_FORCE_INLINE inline 
# endif
#else
#  define CP_BREAKPOINT __builtin_trap()
#  define CP_LIKELY(x)    __builtin_expect(!!(x), 1)
#  define CP_UNLIKELY(x)  __builtin_expect(!!(x), 0) 
#  if defined NDEBUG
#   define CP_FORCE_INLINE __attribute__((always_inline))
#  else
#   define CP_FORCE_INLINE inline
#  endif
#endif