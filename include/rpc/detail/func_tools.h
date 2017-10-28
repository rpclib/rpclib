#pragma once

#ifndef FUNC_TOOLS_H_9FNRD4G2
#define FUNC_TOOLS_H_9FNRD4G2

#include "rpc/detail/all.h"
#include "rpc/detail/any.h"
#include "rpc/detail/invoke.h"

namespace rpc {
namespace detail {

enum class enabled {};

template <typename... C>
using enable_if = invoke<std::enable_if<all<C...>::value, enabled>>;

template <typename... C>
using disable_if = invoke<std::enable_if<!any<C...>::value, enabled>>;

}  // namespace detail
}  // namespace rpc

#endif /* end of include guard: FUNC_TOOLS_H_9FNRD4G2 */
