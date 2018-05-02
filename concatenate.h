#pragma once

//          Copyright Nebojsa Vujnovic 2018-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)


#include "basic_to_string.h"
#include <string>
#include <type_traits>


namespace cp
{
  // this is totally idiotic implementation should be replaced with something with expression templates 
  // that minimizes number of allocation, but at the this time this is something that should enable me to 
  // provide interface for cp::concat function and move on ... 

  std::string concat_impl( ) { return ""; }

  template <typename... Tail >
  std::string concat_impl(Tail const&... t);

  template < typename Head, typename... Tail >
  std::string concat_impl(Head const& h, Tail const&... t)
  { 
    return ::cp::serialization_traits<Head>::to_string(h) + concat_impl(t...);
  }

template <typename... Args>
inline
std::string concat(Args const&... args)
{
  return concat_impl(std::add_const_t<std::add_lvalue_reference_t<std::decay_t<Args>>>(args)... );
}

} // namespace cp
