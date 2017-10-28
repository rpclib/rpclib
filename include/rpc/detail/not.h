#pragma once

#ifndef NOT_H_0MEGQWII
#define NOT_H_0MEGQWII

#include "rpc/detail/bool.h"

namespace rpc {
namespace detail {

template <typename B>
using not_ = bool_<!B::value>;
}
}  // namespace rpc

#endif /* end of include guard: NOT_H_0MEGQWII */
