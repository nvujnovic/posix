#pragma once 

//          Copyright Nebojsa Vujnovic 2018 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "config.h"
#include <system_error>
#include <thread>


namespace cp {
  CP_FORCE_INLINE ::std::error_code 
  make_system_error_code( ) noexcept
  {
    return ::std::error_code{ errno, ::std::system_category() };
  }

  CP_FORCE_INLINE ::std::error_code 
  make_system_error_code( int error_number) noexcept
  {
    return ::std::error_code{ error_number, ::std::system_category() };
  }

  class system_error : public std::system_error 
  {
  public:
    explicit system_error( 
      char const* file, 
      long line, 
      char const* function,
      std::error_code ec,
      ::std::thread::id thread_id = ::std::this_thread::get_id()
    ) 
    : std::system_error{ec} 
    , file(file)
    , line(line)
    , func(function)
    , thr_id(thread_id)
    { }

    explicit system_error( 
      char const* file, 
      long line, 
      char const * function,
      std::error_code ec,
      std::string const& msg, 
      ::std::thread::id thread_id = ::std::this_thread::get_id()
    ) 
    : std::system_error{ec, msg} 
    , file(file)
    , line(line)
    , func(function)
    , thr_id(thread_id)
    { }

    const std::string       file;
    const long              line;
    const std::string       func;
    const std::thread::id   thr_id;
  };
}

#define CP_THROW_SYSTEM_ERROR(x)  do { throw ::cp::system_error(__FILE__, __LINE__ ,__func__, (x) ); } while(0)
#define CP_THROW_SYSTEM_ERROR_MSG(x, msg) do { throw ::cp::system_error(__FILE__, __LINE__, __func__, (x), (msg)); } while(0)
