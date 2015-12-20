#pragma once

#ifndef STRING_REF_H_RTGYQKPI
#define STRING_REF_H_RTGYQKPI

#ifdef _MSC_VER 
#include <string>
#else
#include <experimental/string_view>
#endif

namespace callme {

// TODO: find a string_ref class that works with MSVC [sztomi, 2015-12-20]
#ifdef _MSC_VER 
using string_ref = std::string;
#else
using string_ref = std::experimental::string_view;
#endif

}

#endif /* end of include guard: STRING_REF_H_RTGYQKPI */

