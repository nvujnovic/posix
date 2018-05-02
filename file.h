#pragma once

//          Copyright Nebojsa Vujnovic 2018 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <cstdio>
#include "config.h"
#include "unique_handle.h"
#include "basic_to_string.h"

namespace cp {
  
struct file_traits
{
  static constexpr FILE* invalid(void) noexcept { return nullptr; }
  static void close(FILE* fp) noexcept { std::fclose(fp); }
};

using file = ::cp::unique_handle<FILE*, ::cp::file_traits>;

std::string to_string( file const& f) 
{
  return f ? ::cp::to_string(::cp::to_string(std::uintptr_t(f.get()))) : "invalid";
}

CP_DEFINE_SERIALIZATION_SPECIALIZATION(::cp::file);

} // namespace cp
