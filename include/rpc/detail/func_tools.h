#pragma once

#ifndef FUNC_TOOLS_H_9FNRD4G2
#define FUNC_TOOLS_H_9FNRD4G2

#include "callme/detail/invoke.h"
#include "callme/detail/all.h"
#include "callme/detail/any.h"

namespace callme {
namespace detail {


enum class enabled {};

template <typename... C>
using enable_if = invoke<std::enable_if<all<C...>::value, enabled>>;

template <typename... C>
using disable_if = invoke<std::enable_if<!any<C...>::value, enabled>>;


}
}

#endif /* end of include guard: FUNC_TOOLS_H_9FNRD4G2 */
