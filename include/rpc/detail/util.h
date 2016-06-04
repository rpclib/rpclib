#pragma once

#ifndef UTIL_H_YRIZ63UJ
#define UTIL_H_YRIZ63UJ

#include "msgpack.hpp"

namespace rpc {
namespace detail {
template <typename T> msgpack::object_handle pack(T &&o) {
    auto z = std::make_unique<msgpack::zone>();
    msgpack::object obj(std::forward<T>(o), *z);
    return msgpack::object_handle(obj, std::move(z));
}
} /* detail */
} /* rpc  */

#endif /* end of include guard: UTIL_H_YRIZ63UJ */
