#pragma once

//          Copyright Nebojsa Vujnovic 2018 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <utility>

namespace cp {

// (nebojsa) this class template is something I write whenever I join to new company
// in essence it is just a more flexible unique_ptr

template < typename resource_t, typename traits >
class unique_handle
{
  unique_handle(unique_handle const&) = delete;
  unique_handle& operator = (unique_handle const&) = delete;

  void close() noexcept
  {
    if (*this) {
      traits::close(value_);
    }
  }

public:
  using native_type = resource_t;
  using traits_type = traits;
public:
  unique_handle() noexcept : value_(traits::invalid()) {}

  unique_handle(unique_handle&& other) noexcept
    : value_(other.release())
  { }

  explicit unique_handle(native_type x)
    noexcept(
      noexcept(native_type(std::declval<native_type>()))
      )
    : value_(x) {}
  unique_handle& operator=(unique_handle&& other) noexcept
  {
    reset(other.release());
    return *this;
  }

  explicit operator bool() const noexcept
  {
    return traits::invalid() != value_ ? true : false;
  }

  operator native_type() const noexcept 
  {
    return value_;
  }

  native_type get() const noexcept
  {
    return value_;
  }

  void reset(native_type value = traits::invalid()) noexcept
  {
    if (value_ != value)
    {
      close();
      value_ = value;
    }
  }

  native_type release() noexcept
  {
    auto value = value_;
    value_ = traits::invalid();
    return value;
  }

private:
  native_type value_;
};
} // namespace cp
