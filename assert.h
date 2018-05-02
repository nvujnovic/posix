#pragma once 

//          Copyright Nebojsa Vujnovic 2018 - 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "config.h"

#ifdef NDEBUG
#define CP_ASSERT(x)           ((void)0)
#define CP_ASSERT_MSG(x, msg)  ((void)0)
# else
#define CP_ASSERT(x)           do { if(!(x)) CP_BREAKPOINT; } while(0)
#define CP_ASSERT_MSG(x, msg)  do { if(!(x)) { CP_BREAKPOINT; } } while(0)
#endif