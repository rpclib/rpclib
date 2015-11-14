#pragma once

#ifndef ALL_H_H8MAAYCG
#define ALL_H_H8MAAYCG

#include "callme/detail/invoke.h"
#include "callme/detail/if.h"
#include "callme/detail/bool.h"

namespace callme {
namespace detail {

//! \brief This type can be used to check multiple conditions.
//! It will be true_type if all its arguments are true.
template <typename... T> struct all : true_ {};

template <typename H, typename... T>
struct all<H, T...>
    : if_<H, all<T...>, false_> {};

}
}



#endif /* end of include guard: ALL_H_H8MAAYCG */
