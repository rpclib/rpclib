#pragma once

#ifndef ANY_H_4G3QUOAN
#define ANY_H_4G3QUOAN

#include "callme/detail/invoke.h"
#include "callme/detail/if.h"
#include "callme/detail/bool.h"

namespace callme {
namespace detail {

//! \brief Evaluates to true_type if any of its arguments is true_type.
template <typename... T> struct any : false_ {};

template <typename H, typename... T>
struct any<H, T...> : if_<H, true_, any<T...>> {};
}
}

#endif /* end of include guard: ANY_H_4G3QUOAN */
