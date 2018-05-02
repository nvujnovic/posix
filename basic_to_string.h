#pragma once

//          Copyright Nebojsa Vujnovic 2018 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)


#include <string>
#include <cstring>
#include <algorithm>

namespace cp
{
  // idea is that every type can have uniform serialization to string for need of getting it in the 
  // human readable format
  inline std::string to_string(std::string const& s)     { return s;                       }
  inline std::string to_string(const char*& s)           { return s ? std::string(s) : "nullptr"; }
  inline std::string to_string(char*& s)                 { return s ? std::string(s) : "nullptr"; }

  inline std::string to_string(int value)                { return ::std::to_string(value); }
  inline std::string to_string(long value)               { return ::std::to_string(value); }
  inline std::string to_string(long long value)          { return ::std::to_string(value); }
  inline std::string to_string(unsigned value)           { return ::std::to_string(value); }
  inline std::string to_string(unsigned long value)      { return ::std::to_string(value); }
  inline std::string to_string(unsigned long long value) { return ::std::to_string(value); }
  inline std::string to_string(float value)              { return ::std::to_string(value); }
  inline std::string to_string(double value)             { return ::std::to_string(value); }
  inline std::string to_string(long double value)        { return ::std::to_string(value); }

  template <typename T> struct serialization_traits;

#define CP_DEFINE_SERIALIZATION_SPECIALIZATION( TYPE )     \
  template <>                                              \
  struct serialization_traits<TYPE> {                      \
    static ::std::string to_string(TYPE const&  value) {   \
      return ::cp::to_string(value);                       \
    }                                                      \
  }     

CP_DEFINE_SERIALIZATION_SPECIALIZATION(::std::string);
CP_DEFINE_SERIALIZATION_SPECIALIZATION(const char* );
CP_DEFINE_SERIALIZATION_SPECIALIZATION(char* );
CP_DEFINE_SERIALIZATION_SPECIALIZATION(int);
CP_DEFINE_SERIALIZATION_SPECIALIZATION(unsigned);
CP_DEFINE_SERIALIZATION_SPECIALIZATION(long);
CP_DEFINE_SERIALIZATION_SPECIALIZATION(long long);
CP_DEFINE_SERIALIZATION_SPECIALIZATION(unsigned long);
CP_DEFINE_SERIALIZATION_SPECIALIZATION(unsigned long long);
CP_DEFINE_SERIALIZATION_SPECIALIZATION(float);
CP_DEFINE_SERIALIZATION_SPECIALIZATION(double);
CP_DEFINE_SERIALIZATION_SPECIALIZATION(long double);

}