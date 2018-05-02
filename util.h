#pragma once

//          Copyright Nebojsa Vujnovic 2018 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>
#include <cstdlib>

namespace cp {

// TODO (nebojsa) pointer_cast

struct free_deleter
{
  void operator()(void* x) { std::free(x); }
};

template < typename T>
using unique_malloc_ptr = ::std::unique_ptr<T, ::cp::free_deleter>;

}